<?php
/**
 * @file
 *
 * Generate the civicrm.settings.php file.
 */

if (!defined('CIVI_SETUP')) {
  exit("Installation plugins must only be loaded by the installer.\n");
}

/**
 * Validate the $model.
 */
\Civi\Setup::dispatcher()
  ->addListener('civi.setup.checkRequirements', function(\Civi\Setup\Event\CheckRequirementsEvent $e){
    \Civi\Setup::log()->info(sprintf('[%s] Handle %s', basename(__FILE__), 'checkRequirements'));

    /**
     * @var \Civi\Setup\Model $m
     */
    $m = $e->getModel();

    if (empty($m->settingsPath)) {
      $e->addError('system', 'settingsPath', sprintf('The settingsPath is undefined.'));
    }
    else {
      $e->addInfo('system', 'settingsPath', sprintf('The settingsPath is defined.'));
    }

    if (!\Civi\Setup\FileUtil::isCreateable($m->settingsPath)) {
      $e->addError('system', 'settingsWritable', sprintf('The settings file "%s" cannot be created. Ensure the parent folder is writable.', $m->settingsPath));
    }
    else {
      $e->addInfo('system', 'settingsWritable', sprintf('The settings file "%s" can be created.', $m->settingsPath));
    }
  });

/**
 * Read the $model and create the "civicrm.settings.php".
 */
\Civi\Setup::dispatcher()
  ->addListener('civi.setup.installFiles', function (\Civi\Setup\Event\InstallFilesEvent $e) {
    \Civi\Setup::log()->info(sprintf('[%s] Handle %s', basename(__FILE__), 'installFiles'));

    /**
     * @var \Civi\Setup\Model $m
     */
    $m = $e->getModel();

    // Map from the logical $model to civicrm.settings.php variables.
    $params = array();
    $params['crmRoot'] = addslashes(rtrim($m->srcPath, DIRECTORY_SEPARATOR) . DIRECTORY_SEPARATOR);
    $params['templateCompileDir'] = addslashes($m->templateCompilePath);
    $params['frontEnd'] = 0; // ??why??
    $params['baseURL'] = addslashes(rtrim($m->cmsBaseUrl, '/'));
    $params['dbUser'] = addslashes($m->db['username']);
    $params['dbPass'] = addslashes($m->db['password']);
    $params['dbHost'] = addslashes($m->db['server']);
    $params['dbName'] = addslashes($m->db['database']);
    $params['cms'] = addslashes($m->cms);
    $params['CMSdbUser'] = addslashes($m->cmsDb['username']);
    $params['CMSdbPass'] = addslashes($m->cmsDb['password']);
    $params['CMSdbHost'] = addslashes($m->cmsDb['server']);
    $params['CMSdbName'] = addslashes($m->cmsDb['database']);
    $params['siteKey'] = addslashes($m->siteKey);

    $extraSettings = array();

    foreach ($m->paths as $key => $aspects) {
      foreach ($aspects as $aspect => $value) {
        $extraSettings[] = sprintf('$civicrm_paths[%s][%s] = %s;', var_export($key, 1), var_export($aspect, 1), var_export($value, 1));
      }
    }

    foreach ($m->mandatorySettings as $key => $value) {
      $extraSettings[] = sprintf('$civicrm_setting[%s][%s] = %s;', '\'domain\'', var_export($key, 1), var_export($value, 1));
    }

    // FIXME $m->defaultSettings, $m->components, $m->extensions, $m->callbacks

    if ($extraSettings) {
      $params['extraSettings'] = "Additional settings generated by installer:\n" . implode("\n", $extraSettings);
    }
    else {
      $params['extraSettings'] = "";
    }

    $parent = dirname($m->settingsPath);
    if (!file_exists($parent)) {
      Civi\Setup::log()->info('[InstallSettingsFile.civi-setup.php] mkdir "{path}"', ['path' => $parent]);
      mkdir($parent, 0777, TRUE);
      \Civi\Setup\FileUtil::makeWebWriteable($parent);
    }

    // And persist it...
    $tplPath = implode(DIRECTORY_SEPARATOR,
      [$m->srcPath, 'templates', 'CRM', 'common', 'civicrm.settings.php.template']
    );
    $str = file_get_contents($tplPath);
    foreach ($params as $key => $value) {
      $str = str_replace('%%' . $key . '%%', $value, $str);
    }
    $str = trim($str) . "\n";
    file_put_contents($m->settingsPath, $str);

  }, \Civi\Setup::PRIORITY_LATE);
