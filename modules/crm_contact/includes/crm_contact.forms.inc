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
  $cotnact = $form_state['crm_contact'];

  // Notify field widgets to validate their data.
  field_attach_form_validate('crm_contact', $cotnact, $form, $form_state);
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

/**
 * Form callback: create or edit a contact type.
 *
 * @param $contact_type
 *   The contact type object to edit or for a create form an empty contact type
 *     object with properties instantiated but not populated.
 */
function crm_contact_contact_type_form($form, &$form_state, $contact_type) {
  // Ensure this include file is loaded when the form is rebuilt from the cache.
  $form_state['build_info']['files']['form'] = drupal_get_path('module', 'crm_contact') . '/includes/crm_contact.forms.inc';

  // Store the initial contact type in the form state.
  $form_state['contact_type'] = $contact_type;

  $form['contact_type'] = array(
    '#tree' => TRUE,
  );

  $form['contact_type']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => $contact_type->name,
    '#description' => t('The human-readable name of this contact type. It is recommended that this name begin with a capital letter and contain only letters, numbers, and spaces. This name must be unique.'),
    '#required' => TRUE,
    '#size' => 32,
    '#field_suffix' => ' <small id="edit-contact-type-name-suffix">' . t('Machine name: @type', array('@type' => $contact_type->type)) . '</small>',
  );

  if (empty($contact_type->type)) {
    $form['contact_type']['type'] = array(
      '#type' => 'machine_name',
      '#title' => t('Machine name'),
      '#default_value' => $contact_type->type,
      '#maxlength' => 32,
      '#required' => TRUE,
      '#machine_name' => array(
        'exists' => 'crm_contact_type_load',
        'source' => array('contact_type', 'name'),
      ),
      '#description' => t('The machine-readable name of this contact type. This name must contain only lowercase letters, numbers, and underscores, it must be unique.'),
    );
  }

  $form['contact_type']['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => t('Describe this contact type. The text will be displayed on the <em>Add new content</em> page.'),
    '#default_value' => $contact_type->description,
    '#rows' => 3,
  );

  $form['contact_type']['help'] = array(
    '#type' => 'textarea',
    '#title' => t('Explanation or submission guidelines'),
    '#description' => t('This text will be displayed at the top of the page when creating or editing contacts of this type.'),
    '#default_value' => $contact_type->help,
    '#rows' => 3,
  );

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
    '#value' => t('Save contact type'),
    '#submit' => $submit + array('crm_contact_contact_type_form_submit'),
  );

  if (!empty($form_state['contact_type']->type)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete contact type'),
      '#suffix' => l('Cancel', 'admin/crm/contact/types'),
      '#submit' => $submit + array('crm_contact_contact_type_form_delete_submit'),
      '#weight' => 45,
    );
  }
  else {
    $form['actions']['save_continue'] = array(
      '#type' => 'submit',
      '#value' => t('Save and add fields'),
      '#suffix' => l('Cancel', 'admin/crm/contact/types'),
      '#submit' => $submit + array('crm_contact_contact_type_form_submit'),
      '#weight' => 45,
    );
  }

  $form['#validate'][] = 'crm_contact_contact_type_form_validate';

  return $form;
}

/**
 * Validation callback for crm_contact_contact_type_form().
 */
function crm_contact_contact_type_form_validate($form, &$form_state) {
  $contact_type = $form_state['contact_type'];

  // If saving a new contact type, ensure it has a unique machine name.
  if (empty($contact_type->type)) {
    if (!crm_contact_validate_contact_type_unique($form_state['values']['contact_type']['type'])) {
      form_set_error('contact_type][type', 'The machine name specified is already in use.');
    }
  }
}

/**
 * Form submit handler: save a contact type.
 */
function crm_contact_contact_type_form_submit($form, &$form_state) {
  $contact_type = $form_state['contact_type'];
  $updated = !empty($contact_type->type);

  foreach ($form_state['values']['contact_type'] as $key => $value) {
    $contact_type->$key = $value;
  }

  // Write the contact type to the database.
  $contact_type->is_new = !$updated;
  crm_contact_contact_type_save($contact_type);

  // Redirect based on the button clicked.
  drupal_set_message(t('Contact type saved.'));

  if ($form_state['clicked_button']['#parents'][0] == 'save_continue') {
    $form_state['redirect'] = 'admin/crm/contact/types/' . strtr($form_state['contact_type']->type, array('_' => '-')) . '/fields';
  }
  else {
    $form_state['redirect'] = 'admin/crm/contact/types';
  }
}

