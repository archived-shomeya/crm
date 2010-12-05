<?php
// $Id$

/**
 * @file
 * Provides metadata for the contact entity.
 */

/**
 * Implements hook_entity_property_info().
 */
function crm_contact_entity_property_info() {
  $info = array();

  // Add meta-data about the basic crm_contact properties.
  $properties = &$info['crm_contact']['properties'];

  $properties['cid'] = array(
    'label' => t('Contact ID'),
    'description' => t('The internal numeric ID of the contact.'),
    'type' => 'integer',
  );
  $properties['type'] = array(
    'label' => t('Type'),
    'description' => t('The human readable name of the contact type.'),
    'type' => 'text',
    'options list' => 'crm_contact_type_get_name',
    'required' => TRUE,
    'query callback' => 'entity_metadata_table_query',
  );
  $properties['title'] = array(
    'label' => t('Title'),
    'description' => t('The title of the contact.'),
    'type' => 'text',
    'setter callback' => 'entity_metadata_verbatim_set',
    'required' => TRUE,
    'query callback' => 'entity_metadata_table_query',
  );
  $properties['edit_url'] = array(
    'label' => t('Edit URL'),
    'description' => t("The URL of the contact's edit page."),
    'getter callback' => 'crm_contact_get_properties',
    'type' => 'uri',
  );
  $properties['created'] = array(
    'label' => t('Date created'),
    'description' => t('The date the contact was created.'),
    'type' => 'date',
    'setter callback' => 'entity_metadata_verbatim_set',
    'query callback' => 'entity_metadata_table_query',
    'setter permission' => 'administer contacts',
  );
  $properties['uid'] = array(
    'label' => t("Creator ID"),
    'type' => 'integer',
    'description' => t("The unique ID of the contact creator."),
    'setter callback' => 'entity_metadata_verbatim_set',
    'setter permission' => 'administer contacts',
    'clear' => array('creator'),
    'query callback' => 'entity_metadata_table_query',
  );
  $properties['creator'] = array(
    'label' => t("Creator"),
    'type' => 'user',
    'description' => t("The creator of the contact."),
    'getter callback' => 'crm_contact_get_properties',
    'setter callback' => 'crm_contact_set_properties',
    'setter permission' => 'administer contacts',
    'required' => TRUE,
    'clear' => array('uid'),
  );

  return $info;
}
