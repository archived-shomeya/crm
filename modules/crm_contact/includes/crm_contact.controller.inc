<?php
// $Id$

/**
 * @file
 * The controller for the contact entity containing the CRUD operations.
 */

/**
 * The controller class for contacts contains methods for the contact CRUD
 * operations. The load method is inherited from the default controller.
 */
class CRMContactEntityController extends DrupalDefaultEntityController {

  /**
   * Create a default contact.
   *
   * @param $type
   *   The machine-readable type of the contact.
   *
   * @return
   *   A contact object with all default fields initialized.
   */
  public function create($type = '') {
    return (object) array(
      'cid' => '',
      'vid' => '',
      'is_new' => TRUE,
      'type' => $type,
      'title' => '',
      'uid' => '',
      'created' => '',
      'changed' => '',
    );
  }

  /**
   * Saves a contact.
   *
   * @param $contact
   *   The full contact object to save.
   *
   * @return
   *   The saved contact object.
   */
  public function save($contact) {
    $transaction = db_transaction();

    try {
      $contact->changed = REQUEST_TIME;

      // Save a copy of the unchanged contact for rules in case of updates.
      // see crm_contact_rules_var_unchanged().
      $unchanged = &drupal_static('crm_contact_unchanged', array());
      if (!empty($contact->cid) && (!isset($contact->is_new) || !$contact->is_new)) {
        $unchanged[$contact->cid] = clone crm_contact_load($contact->cid);
      }

      if (!isset($contact->log)) {
        $contact->log = '';
      }

      // Always unset the vid, we'll get a new one when we insert into the 
      // revision table
      $contact->old_vid = $contact->vid;
      unset($contact->vid);

      // Give modules the opportunity to prepare field data for saving.
      field_attach_presave('crm_contact', $contact);
      rules_invoke_all('crm_contact_presave', $contact);

      // If this is a new contact...
      if ((isset($contact->is_new) && $contact->is_new) || empty($contact->cid)) {
        // Set the creation timestamp if not set.
        if (!isset($contact->created) || empty($contact->created)) {
          $contact->created = REQUEST_TIME;
        }

        // Save the new product and fields.
        drupal_write_record('crm_contact', $contact);
        drupal_write_record('crm_contact_revision', $contact);
        field_attach_insert('crm_contact', $contact);

        $op = 'insert';
      }
      else {
        // Save the updated product and fields.
        drupal_write_record('crm_contact', $contact, 'cid');
        // We always save a new revision
        drupal_write_record('crm_contact_revision', $contact);
        field_attach_update('crm_contact', $contact);

        $op = 'update';
      }
      // Since we always save a new revision, update the vid
      db_update('crm_contact')
        ->fields(array('vid' => $contact->vid))
        ->condition('cid', $contact->cid)
        ->execute();

      module_invoke_all('crm_contact_' . $op, $contact);
      module_invoke_all('entity_' . $op, $contact, 'crm_contact');
      rules_invoke_event('crm_contact_' . $op, $contact);

      // Ignore slave server temporarily to give time for the
      // saved product to be propagated to the slave.
      db_ignore_slave();

      return $contact;
    }
    catch (Exception $e) {
      $transaction->rollback();
      watchdog_exception('crm_contact', $e);
      throw $e;
    }
  }

  /**
   * Deletes multiple contacts by ID.
   *
   * @param $contact_ids
   *   An array of contact IDs to delete.
   *
   * @return
   *   TRUE on success, FALSE otherwise.
   */
  public function delete($contact_ids) {
    if (!empty($contact_ids)) {
      $contacts = $this->load($contact_ids, array());

      // Ensure the contacts can actually be deleted.
      foreach ((array) $contacts as $contact_id => $contact) {
        if (in_array(FALSE, module_invoke_all('crm_contact_can_delete', $contact))) {
          unset($contacts[$contact_id]);
        }
      }

      // If none of the specified contacts can be deleted, return FALSE.
      if (empty($contacts)) {
        return FALSE;
      }

      $transaction = db_transaction();

      try {
        db_delete('crm_contact')
          ->condition('cid', $contact_ids, 'IN')
          ->execute();
        db_delete('crm_contact_revision')
          ->condition('cid', $contact_ids, 'IN')
          ->execute();

        foreach ($contacts as $contact_id => $contact) {
          module_invoke_all('crm_contact_delete', $contact);
          field_attach_delete('crm_contact', $contact);
          rules_invoke_event('crm_contact_delete', $contact);
        }

        // Ignore slave server temporarily to give time for the
        // saved contact to be propagated to the slave.
        db_ignore_slave();
      }
      catch (Exception $e) {
        $transaction->rollback();
        watchdog_exception('crm_contact', $e);
        throw $e;
      }

      // Clear the page and block and crm_contact_load_multiple caches.
      cache_clear_all();
      $this->resetCache();
    }

    return TRUE;
  }
}
