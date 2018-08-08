<?php

/**
 * @package CRM
 * @copyright CiviCRM LLC (c) 2004-2018
 *
 * Generated from xml/schema/CRM/Member/Membership.xml
 * DO NOT EDIT.  Generated by CRM_Core_CodeGen
 * (GenCodeChecksum:643f191c50bbd0244e93745a929ba499)
 */

/**
 * Database access object for the Membership entity.
 */
class CRM_Member_DAO_Membership extends CRM_Core_DAO {

  /**
   * Static instance to hold the table name.
   *
   * @var string
   */
  static $_tableName = 'civicrm_membership';

  /**
   * Should CiviCRM log any modifications to this table in the civicrm_log table.
   *
   * @var bool
   */
  static $_log = TRUE;

  /**
   * Membership Id
   *
   * @var int unsigned
   */
  public $id;

  /**
   * FK to Contact ID
   *
   * @var int unsigned
   */
  public $contact_id;

  /**
   * FK to Membership Type
   *
   * @var int unsigned
   */
  public $membership_type_id;

  /**
   * Beginning of initial membership period (member since...).
   *
   * @var date
   */
  public $join_date;

  /**
   * Beginning of current uninterrupted membership period.
   *
   * @var date
   */
  public $start_date;

  /**
   * Current membership period expire date.
   *
   * @var date
   */
  public $end_date;

  /**
   * @var string
   */
  public $source;

  /**
   * FK to Membership Status
   *
   * @var int unsigned
   */
  public $status_id;

  /**
   * Admin users may set a manual status which overrides the calculated status. When this flag is true, automated status update scripts should NOT modify status for the record.
   *
   * @var boolean
   */
  public $is_override;

  /**
   * Then end date of membership status override if 'Override until selected date' override type is selected.
   *
   * @var date
   */
  public $status_override_end_date;

  /**
   * Optional FK to Parent Membership.
   *
   * @var int unsigned
   */
  public $owner_membership_id;

  /**
   * Maximum number of related memberships (membership_type override).
   *
   * @var int
   */
  public $max_related;

  /**
   * @var boolean
   */
  public $is_test;

  /**
   * @var boolean
   */
  public $is_pay_later;

  /**
   * Conditional foreign key to civicrm_contribution_recur id. Each membership in connection with a recurring contribution carries a foreign key to the recurring contribution record. This assumes we can track these processor initiated events.
   *
   * @var int unsigned
   */
  public $contribution_recur_id;

  /**
   * The campaign for which this membership is attached.
   *
   * @var int unsigned
   */
  public $campaign_id;

  /**
   * Class constructor.
   */
  public function __construct() {
    $this->__table = 'civicrm_membership';
    parent::__construct();
  }

  /**
   * Returns foreign keys and entity references.
   *
   * @return array
   *   [CRM_Core_Reference_Interface]
   */
  public static function getReferenceColumns() {
    if (!isset(Civi::$statics[__CLASS__]['links'])) {
      Civi::$statics[__CLASS__]['links'] = static ::createReferenceColumns(__CLASS__);
      Civi::$statics[__CLASS__]['links'][] = new CRM_Core_Reference_Basic(self::getTableName(), 'contact_id', 'civicrm_contact', 'id');
      Civi::$statics[__CLASS__]['links'][] = new CRM_Core_Reference_Basic(self::getTableName(), 'membership_type_id', 'civicrm_membership_type', 'id');
      Civi::$statics[__CLASS__]['links'][] = new CRM_Core_Reference_Basic(self::getTableName(), 'status_id', 'civicrm_membership_status', 'id');
      Civi::$statics[__CLASS__]['links'][] = new CRM_Core_Reference_Basic(self::getTableName(), 'owner_membership_id', 'civicrm_membership', 'id');
      Civi::$statics[__CLASS__]['links'][] = new CRM_Core_Reference_Basic(self::getTableName(), 'contribution_recur_id', 'civicrm_contribution_recur', 'id');
      Civi::$statics[__CLASS__]['links'][] = new CRM_Core_Reference_Basic(self::getTableName(), 'campaign_id', 'civicrm_campaign', 'id');
      CRM_Core_DAO_AllCoreTables::invoke(__CLASS__, 'links_callback', Civi::$statics[__CLASS__]['links']);
    }
    return Civi::$statics[__CLASS__]['links'];
  }

  /**
   * Returns all the column names of this table
   *
   * @return array
   */
  public static function &fields() {
    if (!isset(Civi::$statics[__CLASS__]['fields'])) {
      Civi::$statics[__CLASS__]['fields'] = [
        'membership_id' => [
          'name' => 'id',
          'type' => CRM_Utils_Type::T_INT,
          'title' => ts('Membership ID'),
          'description' => 'Membership Id',
          'required' => TRUE,
          'import' => TRUE,
          'where' => 'civicrm_membership.id',
          'headerPattern' => '/^(m(embership\s)?id)$/i',
          'dataPattern' => '',
          'export' => TRUE,
          'table_name' => 'civicrm_membership',
          'entity' => 'Membership',
          'bao' => 'CRM_Member_BAO_Membership',
          'localizable' => 0,
        ],
        'membership_contact_id' => [
          'name' => 'contact_id',
          'type' => CRM_Utils_Type::T_INT,
          'title' => ts('Contact ID'),
          'description' => 'FK to Contact ID',
          'required' => TRUE,
          'import' => TRUE,
          'where' => 'civicrm_membership.contact_id',
          'headerPattern' => '/contact(.?id)?/i',
          'dataPattern' => '/^\d+$/',
          'export' => TRUE,
          'table_name' => 'civicrm_membership',
          'entity' => 'Membership',
          'bao' => 'CRM_Member_BAO_Membership',
          'localizable' => 0,
          'FKClassName' => 'CRM_Contact_DAO_Contact',
          'html' => [
            'type' => 'EntityRef',
          ],
        ],
        'membership_type_id' => [
          'name' => 'membership_type_id',
          'type' => CRM_Utils_Type::T_INT,
          'title' => ts('Membership Type Id'),
          'description' => 'FK to Membership Type',
          'required' => TRUE,
          'import' => TRUE,
          'where' => 'civicrm_membership.membership_type_id',
          'headerPattern' => '/^(m(embership\s)?type)$/i',
          'dataPattern' => '',
          'export' => FALSE,
          'table_name' => 'civicrm_membership',
          'entity' => 'Membership',
          'bao' => 'CRM_Member_BAO_Membership',
          'localizable' => 0,
          'FKClassName' => 'CRM_Member_DAO_MembershipType',
          'html' => [
            'type' => 'Select',
          ],
          'pseudoconstant' => [
            'table' => 'civicrm_membership_type',
            'keyColumn' => 'id',
            'labelColumn' => 'name',
          ]
        ],
        'join_date' => [
          'name' => 'join_date',
          'type' => CRM_Utils_Type::T_DATE,
          'title' => ts('Member Since'),
          'description' => 'Beginning of initial membership period (member since...).',
          'import' => TRUE,
          'where' => 'civicrm_membership.join_date',
          'headerPattern' => '/^join|(j(oin\s)?date)$/i',
          'dataPattern' => '/\d{4}-?\d{2}-?\d{2}/',
          'export' => TRUE,
          'table_name' => 'civicrm_membership',
          'entity' => 'Membership',
          'bao' => 'CRM_Member_BAO_Membership',
          'localizable' => 0,
          'html' => [
            'type' => 'Select Date',
            'formatType' => 'activityDate',
          ],
        ],
        'membership_start_date' => [
          'name' => 'start_date',
          'type' => CRM_Utils_Type::T_DATE,
          'title' => ts('Membership Start Date'),
          'description' => 'Beginning of current uninterrupted membership period.',
          'import' => TRUE,
          'where' => 'civicrm_membership.start_date',
          'headerPattern' => '/(member(ship)?.)?start(s)?(.date$)?/i',
          'dataPattern' => '/\d{4}-?\d{2}-?\d{2}/',
          'export' => TRUE,
          'table_name' => 'civicrm_membership',
          'entity' => 'Membership',
          'bao' => 'CRM_Member_BAO_Membership',
          'localizable' => 0,
          'html' => [
            'type' => 'Select Date',
            'formatType' => 'activityDate',
          ],
        ],
        'membership_end_date' => [
          'name' => 'end_date',
          'type' => CRM_Utils_Type::T_DATE,
          'title' => ts('Membership Expiration Date'),
          'description' => 'Current membership period expire date.',
          'import' => TRUE,
          'where' => 'civicrm_membership.end_date',
          'headerPattern' => '/(member(ship)?.)?end(s)?(.date$)?/i',
          'dataPattern' => '/\d{4}-?\d{2}-?\d{2}/',
          'export' => TRUE,
          'table_name' => 'civicrm_membership',
          'entity' => 'Membership',
          'bao' => 'CRM_Member_BAO_Membership',
          'localizable' => 0,
          'html' => [
            'type' => 'Select Date',
            'formatType' => 'activityDate',
          ],
        ],
        'membership_source' => [
          'name' => 'source',
          'type' => CRM_Utils_Type::T_STRING,
          'title' => ts('Source'),
          'maxlength' => 128,
          'size' => CRM_Utils_Type::HUGE,
          'import' => TRUE,
          'where' => 'civicrm_membership.source',
          'headerPattern' => '/^(member(ship?))?source$/i',
          'dataPattern' => '',
          'export' => TRUE,
          'table_name' => 'civicrm_membership',
          'entity' => 'Membership',
          'bao' => 'CRM_Member_BAO_Membership',
          'localizable' => 0,
          'html' => [
            'type' => 'Text',
          ],
        ],
        'status_id' => [
          'name' => 'status_id',
          'type' => CRM_Utils_Type::T_INT,
          'title' => ts('Membership Status Id'),
          'description' => 'FK to Membership Status',
          'required' => TRUE,
          'import' => TRUE,
          'where' => 'civicrm_membership.status_id',
          'headerPattern' => '/(member(ship|).)?(status)$/i',
          'dataPattern' => '',
          'export' => FALSE,
          'table_name' => 'civicrm_membership',
          'entity' => 'Membership',
          'bao' => 'CRM_Member_BAO_Membership',
          'localizable' => 0,
          'FKClassName' => 'CRM_Member_DAO_MembershipStatus',
          'html' => [
            'type' => 'Select',
          ],
          'pseudoconstant' => [
            'table' => 'civicrm_membership_status',
            'keyColumn' => 'id',
            'labelColumn' => 'label',
          ]
        ],
        'is_override' => [
          'name' => 'is_override',
          'type' => CRM_Utils_Type::T_BOOLEAN,
          'title' => ts('Status Override'),
          'description' => 'Admin users may set a manual status which overrides the calculated status. When this flag is true, automated status update scripts should NOT modify status for the record.',
          'import' => TRUE,
          'where' => 'civicrm_membership.is_override',
          'headerPattern' => '/override$/i',
          'dataPattern' => '',
          'export' => TRUE,
          'table_name' => 'civicrm_membership',
          'entity' => 'Membership',
          'bao' => 'CRM_Member_BAO_Membership',
          'localizable' => 0,
          'html' => [
            'type' => 'CheckBox',
          ],
        ],
        'status_override_end_date' => [
          'name' => 'status_override_end_date',
          'type' => CRM_Utils_Type::T_DATE,
          'title' => ts('Status Override End Date'),
          'description' => 'Then end date of membership status override if \'Override until selected date\' override type is selected.',
          'import' => TRUE,
          'where' => 'civicrm_membership.status_override_end_date',
          'headerPattern' => '',
          'dataPattern' => '',
          'export' => TRUE,
          'default' => 'NULL',
          'table_name' => 'civicrm_membership',
          'entity' => 'Membership',
          'bao' => 'CRM_Member_BAO_Membership',
          'localizable' => 0,
          'html' => [
            'type' => 'Select Date',
          ],
        ],
        'owner_membership_id' => [
          'name' => 'owner_membership_id',
          'type' => CRM_Utils_Type::T_INT,
          'title' => ts('Primary Member ID'),
          'description' => 'Optional FK to Parent Membership.',
          'export' => TRUE,
          'where' => 'civicrm_membership.owner_membership_id',
          'headerPattern' => '',
          'dataPattern' => '',
          'table_name' => 'civicrm_membership',
          'entity' => 'Membership',
          'bao' => 'CRM_Member_BAO_Membership',
          'localizable' => 0,
          'FKClassName' => 'CRM_Member_DAO_Membership',
        ],
        'max_related' => [
          'name' => 'max_related',
          'type' => CRM_Utils_Type::T_INT,
          'title' => ts('Max Related'),
          'description' => 'Maximum number of related memberships (membership_type override).',
          'table_name' => 'civicrm_membership',
          'entity' => 'Membership',
          'bao' => 'CRM_Member_BAO_Membership',
          'localizable' => 0,
          'html' => [
            'type' => 'Text',
          ],
        ],
        'member_is_test' => [
          'name' => 'is_test',
          'type' => CRM_Utils_Type::T_BOOLEAN,
          'title' => ts('Test'),
          'import' => TRUE,
          'where' => 'civicrm_membership.is_test',
          'headerPattern' => '/(is.)?test(.member(ship)?)?/i',
          'dataPattern' => '',
          'export' => TRUE,
          'table_name' => 'civicrm_membership',
          'entity' => 'Membership',
          'bao' => 'CRM_Member_BAO_Membership',
          'localizable' => 0,
          'html' => [
            'type' => 'CheckBox',
          ],
        ],
        'member_is_pay_later' => [
          'name' => 'is_pay_later',
          'type' => CRM_Utils_Type::T_BOOLEAN,
          'title' => ts('Is Pay Later'),
          'import' => TRUE,
          'where' => 'civicrm_membership.is_pay_later',
          'headerPattern' => '/(is.)?(pay(.)?later)$/i',
          'dataPattern' => '',
          'export' => TRUE,
          'table_name' => 'civicrm_membership',
          'entity' => 'Membership',
          'bao' => 'CRM_Member_BAO_Membership',
          'localizable' => 0,
          'html' => [
            'type' => 'CheckBox',
          ],
        ],
        'contribution_recur_id' => [
          'name' => 'contribution_recur_id',
          'type' => CRM_Utils_Type::T_INT,
          'title' => ts('Membership Recurring Contribution'),
          'description' => 'Conditional foreign key to civicrm_contribution_recur id. Each membership in connection with a recurring contribution carries a foreign key to the recurring contribution record. This assumes we can track these processor initiated events.',
          'table_name' => 'civicrm_membership',
          'entity' => 'Membership',
          'bao' => 'CRM_Member_BAO_Membership',
          'localizable' => 0,
          'FKClassName' => 'CRM_Contribute_DAO_ContributionRecur',
        ],
        'member_campaign_id' => [
          'name' => 'campaign_id',
          'type' => CRM_Utils_Type::T_INT,
          'title' => ts('Campaign'),
          'description' => 'The campaign for which this membership is attached.',
          'import' => TRUE,
          'where' => 'civicrm_membership.campaign_id',
          'headerPattern' => '',
          'dataPattern' => '',
          'export' => TRUE,
          'table_name' => 'civicrm_membership',
          'entity' => 'Membership',
          'bao' => 'CRM_Member_BAO_Membership',
          'localizable' => 0,
          'FKClassName' => 'CRM_Campaign_DAO_Campaign',
          'html' => [
            'type' => 'Select',
          ],
          'pseudoconstant' => [
            'table' => 'civicrm_campaign',
            'keyColumn' => 'id',
            'labelColumn' => 'title',
          ]
        ],
      ];
      CRM_Core_DAO_AllCoreTables::invoke(__CLASS__, 'fields_callback', Civi::$statics[__CLASS__]['fields']);
    }
    return Civi::$statics[__CLASS__]['fields'];
  }

  /**
   * Return a mapping from field-name to the corresponding key (as used in fields()).
   *
   * @return array
   *   Array(string $name => string $uniqueName).
   */
  public static function &fieldKeys() {
    if (!isset(Civi::$statics[__CLASS__]['fieldKeys'])) {
      Civi::$statics[__CLASS__]['fieldKeys'] = array_flip(CRM_Utils_Array::collect('name', self::fields()));
    }
    return Civi::$statics[__CLASS__]['fieldKeys'];
  }

  /**
   * Returns the names of this table
   *
   * @return string
   */
  public static function getTableName() {
    return self::$_tableName;
  }

  /**
   * Returns if this table needs to be logged
   *
   * @return bool
   */
  public function getLog() {
    return self::$_log;
  }

  /**
   * Returns the list of fields that can be imported
   *
   * @param bool $prefix
   *
   * @return array
   */
  public static function &import($prefix = FALSE) {
    $r = CRM_Core_DAO_AllCoreTables::getImports(__CLASS__, 'membership', $prefix, []);
    return $r;
  }

  /**
   * Returns the list of fields that can be exported
   *
   * @param bool $prefix
   *
   * @return array
   */
  public static function &export($prefix = FALSE) {
    $r = CRM_Core_DAO_AllCoreTables::getExports(__CLASS__, 'membership', $prefix, []);
    return $r;
  }

  /**
   * Returns the list of indices
   *
   * @param bool $localize
   *
   * @return array
   */
  public static function indices($localize = TRUE) {
    $indices = [
      'index_owner_membership_id' => [
        'name' => 'index_owner_membership_id',
        'field' => [
          0 => 'owner_membership_id',
        ],
        'localizable' => FALSE,
        'sig' => 'civicrm_membership::0::owner_membership_id',
      ],
    ];
    return ($localize && !empty($indices)) ? CRM_Core_DAO_AllCoreTables::multilingualize(__CLASS__, $indices) : $indices;
  }

}
