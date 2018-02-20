<?php

/**
 * Maximum time an extdir page should be cached (seconds).
 *
 * Note: This overrides the system default.
 */
// define('EXTDIR_MAX_AGE', 30*60);

/**
 * (Page Callback)
 *
 * Get a list of extensions with Git URLs
 */
function extdir_pages_giturls() {
  $query = db_select('node', 'extension')
    ->fields('extension', array('nid'));
  $helper = ExtdirQueryHelper::create($query, array(
    'extension' => 'extension.nid',
  ));

  $helper->innerJoinFieldAPI('extension', 'field_git_url');
  $query->fields('field_git_url', array('field_git_url_value'));

  $helper->innerJoinFieldAPI('extension', 'field_extension_fq_name');
  $query->fields('field_extension_fq_name', array('field_extension_fq_name_value'));

  $helper->innerJoinFieldAPI('extension', 'field_extension_cms');
  $query->condition('field_extension_cms_tid', '127');

  $helper->innerJoinFieldAPI('extension', 'field_extension_ready');
  $query->fields('field_extension_ready', array('field_extension_ready_value'));

  $qr = $query->execute();
  $result = array();
  while ($row = $qr->fetchAssoc()) {
    $result[$row['field_extension_fq_name_value']] = array(
      'key' => $row['field_extension_fq_name_value'],
      'git_url' => $row['field_git_url_value'],
      'ready' => $row['field_extension_ready_value'],
    );
  }
  echo json_encode($result);
  exit();
}

/**
 * (Page Callback)
 *
 * Get list of available extensions
 */
function extdir_pages_listing($filterExpr) {
  $filters = _extdir_pages_filters_parse($filterExpr);
  list($query, $helper) = _extdir_pages_filters_query($filters);
  $helper->innerJoinFieldAPI('extension', 'field_extension_fq_name');
  $query->fields('field_extension_fq_name', array('field_extension_fq_name_value'));
  $query->distinct();

  // deliver
  print '<html><body>';
  print theme('extdir_list', array(
    'extensions' => $query->execute(),
  ));
  print '</body></html>';
  return NULL;
}

/**
 * (Page Callback)
 *
 * Get the metadata for one extension
 */
function extdir_pages_xml($filterExpr, $xmlFile) {
  $filters = _extdir_pages_filters_parse($filterExpr);
  if (!preg_match('/^(.*)\.xml$/', $xmlFile, $matches)) {
    return MENU_NOT_FOUND;
  }
  $filters['field_extension_fq_name_value'] = $matches[1];

  list($query, $helper) = _extdir_pages_filters_query($filters);
  $query->fields('field_extension_nr_crm', array('entity_id', 'field_extension_nr_crm_nid'));
  $helper->innerJoinFieldAPI('release', 'field_extension_release_vc');
  $query->orderBy('field_extension_release_vc_value', 'DESC');
  // $query->orderBy('extrelease.title', 'DESC'); // FIXME dictionary sort != version sort
  $query->range(0,1);

  $count = 0;
  foreach ($query->execute() as $row) {
    $extension = node_load($row->field_extension_nr_crm_nid);
    $release = node_load($row->entity_id);
    if (!file_exists($release->field_extension_release_xml[LANGUAGE_NONE][0]['uri'])) {
      throw new Exception('Failed to read extension XML: file not found: ');
    }
    $xml = simplexml_load_file($release->field_extension_release_xml[LANGUAGE_NONE][0]['uri']);
    _extdir_pages_xml_cleanup($extension, $release, $xml);
    $count++;
  }

  if ($count == 0) {
    return MENU_NOT_FOUND;
  } else {
//    dpm(array('filters' => $filters, 'query' => $query->__toString(), 'extension' => $extension, 'release' => $release, 'xml' => $xml->asXML()));
//    return '';

    // deliver
    $output = $xml->asXML();
    drupal_add_http_header('Content-Type', 'text/xml; utf-8');
    print $output;
    return NULL;

    /*
    $modified = max($extension->changed, $extension->created, $release->changed, $release->created);
    drupal_add_http_header('Content-Type', 'text/xml; utf-8');
    drupal_add_http_header('Expires', gmdate(DATE_RFC1123, REQUEST_TIME + EXTDIR_MAX_AGE));
    drupal_add_http_header('Last-Modified', gmdate(DATE_RFC1123, $modified));
    drupal_add_http_header('Cache-Control', sprintf('max-age=%d, public', EXTDIR_MAX_AGE));
    drupal_add_http_header('ETag', md5($output));
    drupal_page_header();
    print $output;
    exit();
    */
  }
}

/**
 * (Page Callback)
 *
 * Query appropriate extensions and return a JSON file containing
 * XML info files on all of them. Partially based on extdir_pages_listing.
 */
function extdir_pages_single($filterExpr) {
  $filters = _extdir_pages_filters_parse($filterExpr);
  list($query, $helper) = _extdir_pages_filters_query($filters);
  $helper->innerJoinFieldAPI('extension', 'field_extension_fq_name');
  $query->fields('field_extension_fq_name', array('field_extension_fq_name_value'));
  $query->distinct();

  $extensions = array();
  foreach ($query->execute() as $row) {
    $xml_name = $row->field_extension_fq_name_value . '.xml';
    $xml_str = _extdir_pages_single_inner($filterExpr, $xml_name);
    if (!empty($xml_str) && $xml_str != MENU_NOT_FOUND) {
      $extensions[$row->field_extension_fq_name_value] = $xml_str;
    }
  }

  drupal_add_http_header('Content-Type', 'application/json');
  print json_encode($extensions);
  return NULL;
}

/**
 * Called by extdir_pages_single, gets metadata for one extension.
 * Adapted from extdir_pages_xml.
 *
 * @param string $filterExpr
 * @param string $xmlFile
 * @return SimpleXMLElement (or MENU_NOT_FOUND on error)
 * @throws Exception
 */
function _extdir_pages_single_inner($filterExpr, $xmlFile) {
  $filters = _extdir_pages_filters_parse($filterExpr);
  if (!preg_match('/^(.*)\.xml$/', $xmlFile, $matches)) {
    return MENU_NOT_FOUND;
  }
  $filters['field_extension_fq_name_value'] = $matches[1];

  list($query, $helper) = _extdir_pages_filters_query($filters);
  $query->fields('field_extension_nr_crm', array('entity_id', 'field_extension_nr_crm_nid'));
  $helper->innerJoinFieldAPI('release', 'field_extension_release_vc');
  $query->orderBy('field_extension_release_vc_value', 'DESC');
  // $query->orderBy('extrelease.title', 'DESC'); // FIXME dictionary sort != version sort
  $query->range(0,1);

  $count = 0;
  foreach ($query->execute() as $row) {
    $extension = node_load($row->field_extension_nr_crm_nid);
    $release = node_load($row->entity_id);
    if (!file_exists($release->field_extension_release_xml[LANGUAGE_NONE][0]['uri'])) {
      throw new Exception('Failed to read extension XML: file not found: ');
    }
    $xml = simplexml_load_file($release->field_extension_release_xml[LANGUAGE_NONE][0]['uri']);
    if (!is_a($xml, 'SimpleXMLElement')) {
      continue;
    }
    _extdir_pages_xml_cleanup($extension, $release, $xml);
    $count++;
  }

  if ($count == 0) {
    return MENU_NOT_FOUND;
  } else {
    $output = $xml->asXML();
    return $output;
  }
}

/**
 * Modify a raw uploaded XML document, mixing in details
 * from the extension and release.
 *
 * @param $extension stdClass, node
 * @param $release stdClass, node
 * @param $xml SimpleXMLElement, <extension>
 */
function _extdir_pages_xml_cleanup($extension, $release, $xml) {
  // lots of overlap between node and XML; we'll side with the node
  if (!empty($extension->field_extension_fq_name[LANGUAGE_NONE][0]['value'])) {
    $xml['key'] = (string) $extension->field_extension_fq_name[LANGUAGE_NONE][0]['value'];
  }
  if (!empty($release->field_extension_release_url[LANGUAGE_NONE][0]['url'])) {
    $xml->downloadUrl = (string) $release->field_extension_release_url[LANGUAGE_NONE][0]['url'];
  }
  if (!empty($release->field_extension_release_status[LANGUAGE_NONE][0]['value'])) {
    $xml->develStage = (string) $release->field_extension_release_status[LANGUAGE_NONE][0]['value'];
  }
  //if (!empty($release->field_extension_release_ver[LANGUAGE_NONE][0]['value'])) {
  //  $xml->version = (string) $release->field_extension_release_ver[LANGUAGE_NONE][0]['value'];
  //}
  //The title field gets munged, so don't use it
  //if (!empty($release->title)) {
  //  $xml->version = (string) $release->title;
  //}
  $xml->releaseDate = date('Y-m-d', $release->created);
}

/**
 * Parse the filter expression
 *
 * @param $filterExpr string, eg 'ver=4.2.1|cms=Drupal6'
 * @return array, keys:
 *  - field_extension_cms_tid
 *  - field_extension_release_status_value
 *  - field_extension_release_civicrm_tid
 *  - field_extension_ready_value
 */
function _extdir_pages_filters_parse($filterExpr) {
  $args = array();
  $keypairs = explode('|', $filterExpr);
  foreach ($keypairs as $keypair) {
    list ($key, $value) = explode('=', $keypair);
    $args[$key] = $value;
  }

  // defaults
  $filters = array(
    'field_extension_cms_tid' => 127, // Native extension
    'field_extension_release_status_value' => 'stable',
    'field_extension_ready_value' => 'ready',
  );

  // If a "ver" param is provided, override the default filter. The override could be empty (opt-out).
  if (array_key_exists('ver', $args)) {
    $termIds = _extdir_pages_version_tids($args['ver']);
    if (empty($termIds)) {
      $filters['field_extension_release_civicrm_tid'] = -1;
    } else {
      $filters['field_extension_release_civicrm_tid'] = $termIds;
    }
  }

  // If a "status" param is provided, override the default filter. The override could be empty (opt-out).
  if (array_key_exists('status', $args)) {
    if (empty($args['status'])) {
      unset($filters['field_extension_release_status_value']);
    } else {
      // TODO stable, stable+beta, stable+beta+alpha
      $filters['field_extension_release_status_value'] = $args['status'];
    }
  }

  // If a "ready" param is provided, override the default filter. The override could be empty (opt-out).
  if (array_key_exists('ready', $args)) {
    if (empty($args['ready'])) {
      unset($filters['field_extension_ready_value']);
    } else {
      $filters['field_extension_ready_value'] = $args['ready'];
    }
  }

  return $filters;
}

/**
 * Given a user's version of CiviCRM, determine the taxonomy filters that
 * will return compatible extensions.
 *
 * Note that the interpretation changed in v5:
 * - For versions <=4, compatibility tests are exact. Thus,
 *   a site running '4.6.20' would only see extensions
 *   that were tagged `<ver>4.6</ver>`.
 * - For version 5, forward-compatibility is assumed.
 *   Thus, a site running '5.1.0' would see extensions
 *   that were tagged as '<ver>5.0</ver>' or '<ver>5.1</ver>'.
 *
 * @param string $ver
 *   The version of CiviCRM that the user is running.
 *   Ex: '4.6.2', '4.7.10', or '5.2.alpha1'.
 * @return array
 *   List of compatible versions.
 *   Zero or more Drupal taxonomy term IDs.
 */
function _extdir_pages_version_tids($ver) {
  $termNames = array(); // Ex: array('CiviCRM 4.7', 'CiviCRM 5.0', 'CiviCRM 5.1').
  if (version_compare($ver, '5', '<')) {
    list ($a, $b) = explode('.', $ver);
    $termNames[] = sprintf('CiviCRM %d.%d', $a, $b);
  }
  elseif (version_compare($ver, '6', '<')) {
    // The "5.$Y" series is really a continuation of "4.7.$Z".
    $termNames[] = 'CiviCRM 4.7';
    $i = 0;
    while (version_compare("5.$i.alpha", $ver, '<=')) {
      $termNames[] = "CiviCRM 5.$i";
      $i++;
    }
  }
  else {
    throw new \RuntimeException("Failed to filter by unrecognized CiviCRM version");
  }

  $termIds = array();
  foreach ($termNames as $termName) {
    $terms = taxonomy_get_term_by_name($termName, 'extension_civicrm_compatibility');
    foreach ($terms as $term) {
      $termIds[] = $term->tid;
    }
  }

  return $termIds;
}

/**
 * This is utterly ridiculous. Welcome to 2012.
 *
 * EntityFieldQuery doesn't appear to support JOIN, DISTINCT, etc,
 * so we reinvent a slightly more usable wheel.
 *
 * @param $filters array with keys:
 *  - field_extension_cms_tid
 *  - field_extension_release_status_value
 *  - field_extension_release_civicrm_tid
 * @return array(SelectQueryInterface,ExtdirQueryHelper)
 */
function _extdir_pages_filters_query($filters) {
  $query = db_select('node', 'extrelease');
  // two logical entities in this query
  $helper = ExtdirQueryHelper::create($query, array(
    'release' => 'extrelease.nid',
    'extension' => 'field_extension_nr_crm_nid',
  ));
  $helper->innerJoinFieldAPI('release', 'field_extension_nr_crm');

  if (isset($filters['field_extension_cms_tid'])) {
    $helper->innerJoinFieldAPI('extension', 'field_extension_cms');
    $query->condition('field_extension_cms_tid', $filters['field_extension_cms_tid']);
  }

  // FIXME allow releases which are compatible with multiple versions of Civi
  if (isset($filters['field_extension_release_civicrm_tid'])) {
    $helper->innerJoinFieldAPI('release', 'field_extension_release_civicrm');
    $query->condition('field_extension_release_civicrm_tid', $filters['field_extension_release_civicrm_tid']);
  }

  if (isset($filters['field_extension_release_status_value'])) {
    $helper->innerJoinFieldAPI('release', 'field_extension_release_status');
    $query->condition('field_extension_release_status_value', $filters['field_extension_release_status_value']);
  }

  if (isset($filters['field_release_extension_ready_value'])) {
    $helper->innerJoinFieldAPI('release', 'field_release_extension_ready');
    $query->condition('field_release_extension_ready_value', $filters['field_release_extension_ready_value']);
  }

  if (isset($filters['field_extension_ready_value'])) {
    $helper->innerJoinFieldAPI('extension', 'field_extension_ready');
    $query->condition('field_extension_ready_value', $filters['field_extension_ready_value']);
  }

  if (isset($filters['field_extension_fq_name_value'])) {
    $helper->innerJoinFieldAPI('extension', 'field_extension_fq_name');
    $query->condition('field_extension_fq_name_value', $filters['field_extension_fq_name_value']);
  }

  return array($query, $helper);
}

/**
 * Helper for mixing in DB columns generated by
 * Field API's SQL storage engine -- which works
 * with multiple entities, JOINs, GROUP BYs, ec
 */
class ExtdirQueryHelper {
  /**
   * @param $query SelectQueryInterface
   * @parm $entityAliases array($niceName => $sqlIdExpression) list of entities
   *   which are (mentally) part of the query -- and the SQL expressions which
   *   identify them
   */
  static function create($query, $entityAliases) {
    return new ExtdirQueryHelper($query, $entityAliases);
  }
  function __construct($query, $entityAliases) {
    $this->query = $query;
    $this->entityAliases = $entityAliases;
  }
  function innerJoinFieldAPI($entityAlias, $joinToField, $joinToAlias = NULL, $joinToColumn = 'entity_id') {
    return $this->joinFieldAPI('INNER', $entityAlias, $joinToField, $joinToAlias, $joinToColumn);
  }
  function joinFieldAPI($type, $entityAlias, $joinToField, $joinToAlias = NULL, $joinToColumn = 'entity_id') {
    if ($joinToAlias === NULL) {
      $joinToAlias = $joinToField;
    }

    $fieldInfo = field_info_field($joinToField);
    // assumes that field uses one table
    foreach ($fieldInfo['storage']['details']['sql']['FIELD_LOAD_CURRENT'] as $table => $columns) {
      $condition = $this->entityAliases[$entityAlias]." = ${joinToAlias}.${joinToColumn}";
      $this->query->addJoin($type, $table, $joinToAlias, $condition);
    }
    return $this;
  }
}

