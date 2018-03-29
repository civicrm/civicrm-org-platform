<?php
/*
 +--------------------------------------------------------------------+
 | CiviCRM version 4.4                                                |
 +--------------------------------------------------------------------+
 | Copyright CiviCRM LLC (c) 2004-2013                                |
 +--------------------------------------------------------------------+
 | This file is a part of CiviCRM.                                    |
 |                                                                    |
 | CiviCRM is free software; you can copy, modify, and distribute it  |
 | under the terms of the GNU Affero General Public License           |
 | Version 3, 19 November 2007 and the CiviCRM Licensing Exception.   |
 |                                                                    |
 | CiviCRM is distributed in the hope that it will be useful, but     |
 | WITHOUT ANY WARRANTY; without even the implied warranty of         |
 | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.               |
 | See the GNU Affero General Public License for more details.        |
 |                                                                    |
 | You should have received a copy of the GNU Affero General Public   |
 | License and the CiviCRM Licensing Exception along                  |
 | with this program; if not, contact CiviCRM LLC                     |
 | at info[AT]civicrm[DOT]org. If you have questions about the        |
 | GNU Affero General Public License or the licensing of CiviCRM,     |
 | see the CiviCRM license FAQ at http://civicrm.org/licensing        |
 +--------------------------------------------------------------------+
*/

/**
 *
 * @package CRM
 * @copyright CiviCRM LLC (c) 2004-2013
 * $Id$
 *
 */

/**
 * @file
 * Example drush command.
 *
 * Shows how to make your own drush command.
 *
 * You can copy this file to any of the following
 * 1. A .drush folder in your HOME folder.
 * 2. Anywhere in a folder tree below an active module on your site.
 * 3. In an arbitrary folder specified with the --include option.
 */

/**
 * Implementation of hook_drush_command().
 *
 * In this hook, you specify which commands your
 * drush module makes available, what it does and
 * description.
 *
 * Notice how this structure closely resembles how
 * you define menu hooks.
 *
 * @See drush_parse_command() for a list of recognized keys.
 *
 * @return
 * An associative array describing your command(s).
 */
function extdir_drush_command() {
  $items = array();

  $items['extdir-scan'] = array(
    'description' => "Scans the git repositories of published extensions for new releases",
    'examples' => array(
      'Standard example' => 'drush extdir-scan',
    ),
    'arguments' => array('pattern' => 'Regular expression for extensions to process (Optional)'),
    'options' => array(
      'test' => 'Test mode. (Do not write to DB or send emails)',
    ),
    'aliases' => array('exs'),
  );

  return $items;
}

/**
 * (Drush callback)
 *
 * Implementation of command 'extdir-scan'
 */
function drush_extdir_scan($pattern = NULL) {
  // Load dependencies in other modules
  module_load_include('pages.inc', 'extdir');

  // Get the list of all published CMS-independent extensions
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
    ->propertyCondition('type', 'extension')  // Extension
    ->propertyCondition('status', '1')        // Published
    ->fieldCondition('field_extension_cms', 'tid', '127'); // CMS-independant
  $result = $query->execute();
  $ids = array_keys($result['node']);
  echo sizeof($ids).' CMS-independent extensions found'.PHP_EOL;

  // Foreach Extension
  foreach ($ids as $id) {
    $extension = node_load($id);
    if ($pattern && !preg_match($pattern, $extension->title)) {
      continue;
    }
    _extdir_debug(sprintf("Processing extension \"%s\" (%s)\n", $extension->title, $id));

    // Get email addresses of maintainers
    $maintainers = _extdir_get_maintainers($extension);
    _extdir_debug(array('maintainers' => $maintainers));

    // Get existing releases from database
    list($releases, $lastRelease) = _extdir_get_releases($id);
    _extdir_debug(array('releases' => $releases));

    // Get sources releases from git tags
    $giturl = $extension->field_git_url[LANGUAGE_NONE][0]['value'];
    if (empty($giturl)) {
      _extdir_debug(sprintf("Skip extension \"%s\" (#%s) due to missing Git URL\n", $extension->title, $id));
      continue;
      // $giturl = _extdir_guess_git_url($lastRelease, $extension);
    }
    $gittags = _extdir_get_tags($giturl);
    _extdir_debug(array('giturl' => $giturl, 'gittags' => $gittags));

    // Foreach source release, create release if needed and possible
    foreach ($gittags as $version => $tag) {
      // Is this release not yet created on civicrm.org?
      if (!in_array($version, $releases)) {
        list($errorMessages, $release) = _extdir_create_release($extension, $version, $giturl, $tag);
        // Have we had any errors in the validation process?
        if ($release != NULL && empty($errorMessages)) {
          printf("Create release for %s %s (%s @ %s)\n", $extension->title, $version, $giturl, $tag);
          _extdir_debug(array('new-release' => $release));
          if (! drush_get_option('test')) {
            $release = node_submit($release);
            node_save($release);
          }
        } else {
          printf("Skip release of %s %s (%s @ %s) due to %d error(s)\n", $extension->title, $version, $giturl, $tag, count($errorMessages));
        }

        // Format and send email with messages
        $body = _extdir_compose_email($tag, $extension, $errorMessages);
        printf("Send email (%s)\n", implode($maintainers, ', '));
        _extdir_debug(array('email-body' => $body));
        if (! drush_get_option('test')) {
          _extdir_send_email(implode($maintainers, ', '), t('Your extension release on civicrm.org'), $body);
        }
      } // end if version not in existing releases
    } // end foreach git tag
  } // end foreach extension
}

function _extdir_debug($mixed) {
  if (drush_get_option('test')) {
    print_r($mixed);
  }
}

/**
 * @param stdClass<Extension> $extension
 * @param string $version
 * @param string $giturl
 * @param string $tag
 * @return array (0 => array<String> $errorMessages, 1 => stdClass<ExtensionRelease> $release)
 */
function _extdir_create_release($extension, $version, $giturl, $tag) {
  $errorMessages = array();
  $info = _extdir_git_getfile($giturl, $tag, 'info.xml');
  // Have we been able to get the XML and is it valid?
  if ($info && ($xml = simplexml_load_string($info))) {
    // Create the extension release node
    $release = new stdClass();
    $release->title = "Version $version";
    $release->type = 'extension_release_civicrm';
    node_object_prepare($release);
    $release->language = LANGUAGE_NONE;
    $release->status = 1; //(1 or 0): published or not
    $release->promote = 0; //(1 or 0): promoted to front page
    $release->comment = 1; // 0 = comments disabled, 1 = read only, 2 = read/write
    // Add parent extension node
    $release->field_extension_nr_crm[LANGUAGE_NONE][0]['nid'] = $extension->nid;
    // Add creator as first in the list of maintainer
    if (sizeof($extension->field_maintainers[LANGUAGE_NONE])) {
      $release->uid = $extension->field_maintainers[LANGUAGE_NONE][0]['target_id'];
    }
    else {
      $release->uid = 1;
    }
    $release->revision_uid = $release->uid;
    // Add maintainers
    $release->field_extension_release_ci_mains[LANGUAGE_NONE] = $extension->field_maintainers[LANGUAGE_NONE];
    // Add downloadURL
    $release->field_extension_release_url[LANGUAGE_NONE][0]['url'] = _extdir_git_getdurl($giturl, $tag);
    // Add develStage
    $stage = 'stable';
    foreach (array('alpha', 'beta') as $mark) {
      if (strpos($tag, $mark) !== FALSE) {
        $stage = $mark;
      }
    }
    $release->field_extension_release_status[LANGUAGE_NONE][0]['value'] = $stage;
    // Add XML file (ie. we want to have the raw here so additional logic can be added at distribution)
    $file = file_save_data($xml->asXML());
    $file->display = 1;
    $file->filename = 'info.xml';
    $release->field_extension_release_xml[LANGUAGE_NONE][0] = (array) $file;
    // Validate the XML tags (content in taxonomy, mandatory values set, etc.)
    // 1. check that the XML does NOT include tags that should be empty
    foreach (array('downloadUrl', 'develStage') as $path) {
      if ($xml->xpath("/extension/$path")) {
        // *** DISABLED *** while we are grandfathering older extensions
        // $messages[] = t('The XML attribute @path should not be present in the info.xml', array('@path' => $path));
      }
    }
    // 2. Switch the format to DOMDocument() as this is what's used by the form submission checks
    libxml_use_internal_errors(TRUE);
    $doc = new DOMDocument();
    $doc->recover = TRUE;
    $doc->strictErrorChecking = FALSE;
    if ($doc->load(drupal_realpath($file->uri))) {
      // Use the form submission checks to validate values/conformance of XML tags
      $keys = array('version', 'releaseDate', 'ver', 'extension', 'documentation');
      foreach ($keys as $xmltag) {
        $tagcontents = FALSE;
        $error = _ccrmextvalid_validateTag($doc, $xmltag, $tagcontents, $extension);
        if (empty($error)) {
          _extdir_assignTag($release, $xmltag, $tagcontents);
        }
        else {
          $errorMessages[] = $error;
        }
      }
      // One last sanity check on the release number
      if ($version != $release->field_extension_release_version[LANGUAGE_NONE][0]['value']) {
        $errorMessages[] = t('The version numbers in the info.xml ("@infoxml") and in the git tag ("@tag") do not match.', array(
          '@infoxml' => $release->field_extension_release_version[LANGUAGE_NONE][0]['value'],
          '@tag' => $version,
        ));
        return array($errorMessages, $release);
      }
      return array($errorMessages, $release);
    }
    else {
      $errorMessages[] = t('There was an error loading a DOMDocument from "@uri".', array(
        '@uri' => $file->uri,
      ));
      return array($errorMessages, $release);
    }
  }
  else {
    $release = NULL;
    if ($info == FALSE) {
      if (strpos($giturl, 'https://github.com') === FALSE && strpos($giturl, 'https://lab.civicrm.org/extensions') === FALSE) {
        $errorMessages[] = t('We are unable to automatically process releases if your extension is not on lab.civicrm.org/extensions or github.com.');
        return array($errorMessages, $release);
      }
      else {
        $errorMessages[] = t('We were unable to find an info.xml in the root directory of this release.');
        return array($errorMessages, $release);
      }
    }
    else {
      $errorMessages[] = t('The info.xml you provided in this release is not valid.');
      return array($errorMessages, $release);
    }
  }
}

/**
 * Get a list of tags in a git repo
 *
 * @param string|NULL $giturl
 * @return array (string $version => string $tag) list of tags. if $giturl is missing/invalid, then array()
 */
function _extdir_get_tags($giturl) {
  $gittags = array();
  if (!empty($giturl)) {
    $output = array();
    $cmd = sprintf("git ls-remote --tags %s", escapeshellarg($giturl));
    exec($cmd, $output);
    foreach ($output as $line) {
      if ($pos = strpos($line, 'tags/')) {
        $tag = substr($line, $pos + 5);
        if (preg_match('/^v?[0-9](0|1|2|3|4|5|6|7|8|9|\.|-|alpha|beta)*$/', $tag)) {
          $version = str_replace('v', '', $tag);
          $gittags[$version] = $tag;
        }
      }
    }
  }
  return $gittags;
}

/**
 * Guess the URL of a git repo based on a download URL
 *
 * @param stdClass<ExtensionRelease> $release
 * @param stdClass<Extension> $extension
 * @return array|string
 */
function _extdir_guess_git_url($release, $extension) {
  $relurl = $release->field_extension_release_url[LANGUAGE_NONE][0]['url'];
  if ($pos = strpos($relurl, '/archive')) {
    // GitHub naming schema
    $giturl = substr($relurl, 0, $pos) . '.git';
  }
  // Save URL back to Extension node - TO BE DELETED after move to production
  if (!empty($giturl)) {

    $extension->field_extension_git_url = $giturl;
    node_save($extension);
    return $giturl;
  }
  return $giturl;
}

/**
 * @param string $tag
 * @param stdClass<Extension> $extension
 * @param array<String> $messages
 * @return string
 */
function _extdir_compose_email($tag, $extension, $errorMessages) {
  $body = t('This is a message from the automated release system on civicrm.org.') . ' ';
  $body .= t('Congratulations on releasing version @tag of extension \'@ext\'.',
    array('@tag' => $tag, '@ext' => $extension->title)) . "\n\n";
  if (sizeof($errorMessages)) {
    $body .= t('Unfortunately we have found a few errors while processing your release:') . "\n";
    foreach ($errorMessages as $line) {
      $body .= "  - $line\n";
    }
    $body .= "\n" . t('You will need to correct these errors before we can accept this release.') . "\n"
      . "\n"
      . t('Further help is available in the developer guide at @url.',
        array('@url' => 'https://docs.civicrm.org/dev/en/latest/extensions/publish/')) . "\n"
      . "\n"
      . t('You can also request help on Mattermost at @url.',
        array('@url' => 'https://chat.civicrm.org/civicrm/channels/extensions')) . "\n"
      . "\n" . t("If you no longer wish to publish this release, deleting the @tag tag from your extension's repository will stop these notices.",
        array('@tag' => $tag)) . "\n";
  }
  else {
    $body .= t('Your release was successfully processed and is now live on the civicrm.org extensions directory.') . "\n";
  }
  $body .= "\n" . t('Thank you so much for being an active contributor to CiviCRM and our community.') . "\n";
  return $body;
}

/**
 * @param int $id nid of the extension node
 * @return array (0 => array<String>, 1 => NULL|stdClass<ExtensionRelease>)
 */
function _extdir_get_releases($id) {
  $releases = array();
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
    ->propertyCondition('type', 'extension_release_civicrm')
    ->propertyCondition('status', '1')
    ->fieldCondition('field_extension_nr_crm', 'nid', $id);
  $result = $query->execute();
  if (sizeof($result['node'])) {
    $relids = array_keys($result['node']);
    foreach ($relids as $relid) {
      $release = node_load($relid);
      $version = $release->field_extension_release_version[LANGUAGE_NONE][0]['value'];
      $releases[] = str_replace('Version ', '', $version);
    }
    return array($releases, $release);
  }
  return array($releases, NULL);
}

/**
 * @param stdClass<Extension> $extension
 * @return array<String> email addresses
 */
function _extdir_get_maintainers($extension) {
  $maintainers = array();
  if (sizeof($extension->field_maintainers[LANGUAGE_NONE])) {
    foreach ($extension->field_maintainers[LANGUAGE_NONE] as $user) {
      $maintainer = user_load($user['target_id']);
      if ($maintainer->mail) {
        $maintainers[] = $maintainer->mail;
      }
    }
    return $maintainers;
  }
  return $maintainers;
}

/**
 * Reads a single file from a git repository at a given release
 * Returns either the file content or false on failure
 * It WILL fail if the repository does not allow such fetching
 *   - only lab.civicrm.org/extensions and github.com are supported
 *   - other git repos might be supported with the git archive command but this requires server components
 */
function _extdir_git_getfile($giturl, $tag, $file) {
  if (preg_match(';(https|git)://(github.com|lab.civicrm.org)/;', $giturl)) {
    $giturl = rtrim($giturl, '/');
    $giturl = preg_replace(':\.git$:', '', $giturl);
    $giturl = preg_replace(';(https|git)://github.com/;', 'https://raw.githubusercontent.com/', $giturl);
    $giturl = preg_replace(';(https|git)://lab.civicrm.org/extensions/(.*);', 'https://lab.civicrm.org/extensions/$2/raw', $giturl);
    $giturl .= "/$tag/$file";
    return file_get_contents($giturl);
  }
  return FALSE;
}

function _extdir_git_getdurl($giturl, $tag) {
  if (substr($giturl, 0, 19) == 'https://github.com/') {
    $projUrl = $giturl;
    if (preg_match('/\.git$/', $projUrl)) {
      // In github URLs, the '.git' extension is optional.
      $projUrl = substr($giturl, 0, -4);
    }
    return "$projUrl/archive/$tag.zip";
  }
  return FALSE;
}

/**
 * Fills in the tag values in the Extension Release node
 * copied from _ccrmextvalid_assignTag($form, &$form_state, $tag, &$tagcontents)
 */
function _extdir_assignTag(&$release, $tag, $tagcontents) {
  switch ($tag) {
    case 'version':
      module_load_include('version.inc', 'ccrm_extensionvalidation');
      $release->field_extension_release_version[LANGUAGE_NONE][0]['value'] = $tagcontents;
      $release->field_extension_release_vc[LANGUAGE_NONE][0]['value'] = ccrm_extensionvalidation_version_normalize($tagcontents);
      break;
    case 'releaseDate':
      $release->field_extension_release_date[LANGUAGE_NONE][0]['value'] = $tagcontents;
      break;
    case 'develStage':
      $release->field_extension_release_status[LANGUAGE_NONE][0]['value'] = $tagcontents;
      break;
    case 'downloadUrl':
      $release->field_extension_release_url[LANGUAGE_NONE][0]['url'] = $tagcontents;
      break;
    case 'ver':
      $release->field_extension_release_civicrm[LANGUAGE_NONE] = $tagcontents;
      break;
    case 'documentation':
      $release->field_documentation[LANGUAGE_NONE][0]['url'] = $tagcontents;
      break;
  }
}

// Shamelessly copied from https://drupal.org/sandbox/jfhovinne/2131795
function _extdir_send_email($to, $subject = 'Test message', $body = 'This is a test.') {
  // Define $from and headers
  if (!$from = drush_get_option('from')) $from = variable_get('site_mail', ini_get('sendmail_from'));
  $headers = array();
  $headers['From'] = $headers['Sender'] = $headers['Return-Path'] = $headers['Errors-To'] = $from;

  $message = drupal_mail('drush', 'extdir', $to, NULL, array(), $from, FALSE);

  $message['subject'] = $subject;
  $message['body'] = array();
  $message['body'][] = $body;
  $message['headers'] = $headers;

  // Retrieve the responsible implementation for this message.
  $system = drupal_mail_system('drush', 'extdir');
  // Format the message body.
  $message = $system->format($message);
  // Send e-mail.
  if ($system->mail($message)) {
    //echo "Email sent to: $to".PHP_EOL;
  } else {
    print_r(array('Failed to send message' => $message));
  }
  return $status;
}
