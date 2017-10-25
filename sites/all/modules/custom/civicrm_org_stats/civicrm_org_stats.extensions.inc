<?php
/**
 * @file
 * Code for the CiviCRM Extensions Stats module.
 */
require_once("civicrm_org_stats.inc");
class civicrm_org_stats_extensions extends civicrm_org_stats {

  private $stats = array();
  const URL = "https://stats.civicrm.org/json/extensions-detail.json";

  public function fetch() {
    $returnedRawValues = drupal_http_request(self::URL);
    if (!empty($returnedRawValues->data)) {
      $decodedValues = json_decode($returnedRawValues->data, TRUE);
    }

    if (!empty($decodedValues)) {
      foreach ($decodedValues as $val) {
        if (!empty($val['name'])) {
          $this->stats[$val['name']] = (int) $val['num_sites'];
        }
      }
      return TRUE;
    }
    return FALSE;
  }

  public function update() {
    $nodes = db_query("
      SELECT fq.entity_id as nid, fq.field_extension_fq_name_value as name, fq.language, fq.revision_id, cu.entity_id as cu_node, cu.field_extension_current_usage_value as usage_value
      FROM field_data_field_extension_fq_name fq
      LEFT JOIN field_data_field_extension_current_usage cu ON cu.entity_id = fq.entity_id
      WHERE fq.deleted = 0
      AND fq.field_extension_fq_name_value IN (:keys)",
      array(':keys' => array_keys($this->stats))
    );
    foreach ($nodes as $node) {
      $usage = $this->stats[$node->name];
      // No need to update
      if (!empty($node->cu_node) && $node->usage_value == $usage) {
        continue;
      }
      $clear_cache = TRUE;
      // Update existing field
      if (!empty($node->cu_node)) {
        $this->updateRecord($node, $usage);
      }
      // Insert if necessary
      else {
        $this->insertRecord($node, $usage);
      }
    }
    if (isset($clear_cache)) {
      $this->clearCache();
    }
  }

}
