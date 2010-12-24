<?php
// $Id$

/**
 * @file
 * Forms for creating, editing, and deleting contacts.
 */

function crm_contact_add_page() {
  $contact = crm_contact_new();
  $contact->type = 'default';
  return drupal_get_form('crm_contact_contact_form', $contact);
}

/**
 * Form callback: create or edit a contact.
 *
 * @param $contact
 *   The contact object to edit or for a create form an empty contact object
 *     with only a contact type defined.
 */
function crm_contact_contact_form($form, &$form_state, $contact) {
  // Ensure this include file is loaded when the form is rebuilt from the cache.
  $form_state['build_info']['files']['form'] = drupal_get_path('module', 'crm_contact') . '/includes/crm_contact.forms.inc';

  // Add the default field elements.
  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#default_value' => $contact->title,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#weight' => -5,
  );

  // Add the field related form elements.
  $form_state['crm_contact'] = $contact;
  field_attach_form('crm_contact', $contact, $form, $form_state);

  $form['actions'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('form-actions')),
    '#weight' => 40,
  );

  // We add the form's #submit array to this button along with the actual submit
  // handler to preserve any submit handlers added by a form callback_wrapper.
  $submit = array();

  if (!empty($form['#submit'])) {
    $submit += $form['#submit'];
  }

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save contact'),
    '#submit' => $submit + array('crm_contact_contact_form_submit'),
  );

  // We append the validate handler to #validate in case a form callback_wrapper
  // is used to add validate handlers earlier.
  $form['#validate'][] = 'crm_contact_contact_form_validate';

  return $form;
}

/**
 * Validation callback for crm_contact_contact_form().
 */
function crm_contact_contact_form_validate($form, &$form_state) {
  $contact = $form_state['crm_contact'];

  // Notify field widgets to validate their data.
  field_attach_form_validate('crm_contact', $contact, $form, $form_state);
}

/**
 * Submit callback for crm_contact_contact_form().
 */
function crm_contact_contact_form_submit($form, &$form_state) {
  global $user;

  $contact = &$form_state['crm_contact'];

  // Save default parameters back into the $contact object.
  $contact->title = $form_state['values']['title'];

  // Set the contact's uid if it's being created at this time.
  if (empty($contact->cid)) {
    $contact->uid = $user->uid;
  }

  // Notify field widgets.
  field_attach_submit('crm_contact', $contact, $form, $form_state);

  // Save the contact.
  crm_contact_save($contact);

  // Redirect based on the button clicked.
  drupal_set_message(t('Contact saved.'));
}

/**
 * Form callback: confirmation form for deleting a contact.
 *
 * @param $contact
 *   The contact object to be deleted.
 *
 * @see confirm_form()
 */
function crm_contact_contact_delete_form($form, &$form_state, $contact) {
  $form_state['crm_contact'] = $contact;

  // Ensure this include file is loaded when the form is rebuilt from the cache.
  $form_state['build_info']['files']['form'] = drupal_get_path('module', 'crm_contact') . '/includes/crm_contact.forms.inc';

  $form['#submit'][] = 'crm_contact_contact_delete_form_submit';

  $form = confirm_form($form,
    t('Are you sure you want to delete %title?', array('%title' => $contact->title)),
    '',
    '<p>' . t('Deleting this contact cannot be undone.') . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Submit callback for crm_contact_contact_delete_form().
 */
function crm_contact_contact_delete_form_submit($form, &$form_state) {
  $contact = $form_state['crm_contact'];

  if (crm_contact_delete($contact->cid)) {
    drupal_set_message(t('%title has been deleted.', array('%title' => $contact->title)));
    watchdog('crm_contact', 'Deleted contact %title.', array('%title' => $contact->title), WATCHDOG_NOTICE);
  }
  else {
    drupal_set_message(t('%title could not be deleted.', array('%title' => $contact->title)), 'error');
  }
}
