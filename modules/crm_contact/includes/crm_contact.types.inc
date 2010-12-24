<?php
// $Id$

/**
 * @file
 */

/**
 * Menu callback: display an overview of available types.
 */
function crm_contact_ui_types_overview() {
//  drupal_add_css(drupal_get_path('module', 'commerce_product_ui') . '/theme/commerce_product_ui.types.css');

  $header = array(
    t('Name'),
    t('Operations'),
  );

  $rows = array();

  // Loop through all defined contact types.
  foreach (crm_contact_types() as $type => $contact_type) {
    // Build the operation links for the current contact type.
    $links = menu_contextual_links('crm-contact-type', 'admin/crm/contact/types', array(strtr($type, array('_' => '-'))));

    // Add the contact type's row to the table's rows array.
    $rows[] = array(
      theme('contact_type_admin_overview', array('contact_type' => $contact_type)),
      theme('links', array('links' => $links, 'attributes' => array('class' => 'links inline operations'))),
    );
  }

  // If no contact types are defined...
  if (empty($rows)) {
    // Add a standard empty row with a link to add a new contact type.
    $rows[] = array(
      array(
        'data' => t('There are no contact types yet. <a href="@link">Add contact type</a>.', array('@link' => url('admin/crm/contact/types/add'))),
        'colspan' => 2,
      )
    );
  }

  return theme('table', array('header' => $header, 'rows' => $rows));
}

/**
 * Form callback wrapper: create or edit a contact type.
 *
 * @param $contact_type
 *   The contact type object being edited by this form.
 *
 * @see crm_contact_contact_type_form()
 */
function crm_contact_contact_type_form_wrapper($contact_type) {
  // Add the breadcrumb for the form's location.
//  commerce_product_ui_set_breadcrumb(TRUE);

  // Include the forms file from the contact module.
  module_load_include('inc', 'crm_contact', 'includes/crm_contact.forms');

  return drupal_get_form('crm_contact_contact_type_form', $contact_type);
}


/**
 * Builds an overview of a contact type for display to an administrator.
 *
 * @param $variables
 *   An array of variables used to generate the display; by default includes the
 *     type key with a value of the contact type object.
 *
 * @ingroup themeable
 */
function theme_contact_type_admin_overview($variables) {
  $contact_type = $variables['contact_type'];

  $output = check_plain($contact_type->name);
  $output .= ' <small> (Machine name: ' . check_plain($contact_type->type) . ')</small>';
  $output .= '<div class="description">' . filter_xss_admin($contact_type->description) . '</div>';

  return $output;
}

/**
 * Form callback wrapper: confirmation form for deleting a contact type.
 *
 * @param $contact_type
 *   The contact type object being deleted by this form.
 *
 * @see crm_contact_contact_type_delete_form()
 */
function crm_contact_contact_type_delete_form_wrapper($contact_type) {
  // Add the breadcrumb for the form's location.
//  commerce_product_ui_set_breadcrumb(TRUE);

  // Don't allow deletion of contact types that have contacts already.
  if (($count = db_query("SELECT cid FROM {crm_contact} WHERE type = :contact_type", array(':contact_type' => $contact_type->type))->rowCount()) > 0) {
    drupal_set_title(t('Cannot delete the %name contact type', array('%name' => $contact_type->name)), PASS_THROUGH);
    return t('There are @count contacts of this type. It cannot be deleted.', array('@count' => $count));
  }

  // Include the forms file from the contact module.
  module_load_include('inc', 'crm_contact', 'includes/crm_contact.forms');

  return drupal_get_form('crm_contact_contact_type_delete_form', $contact_type);
}