<?php

/**
 * @file
 * Provides administration forms for webform_nodereference module.
 */

/**
 * Return the Webform Node Reference settings form.
 */
function webform_nodereference_admin_settings_form() {
  return array(
    'rebuild_count' => array(
      'submit' => array(
        '#type' => 'submit',
        '#value' => 'Rebuild count index',
      )
    ),
  );
}

/**
 * Submit Handler; Webform Node Reference Rebuild Count Index
 */
function webform_nodereference_admin_settings_form_submit($form, &$form_state) {
  db_query("TRUNCATE {webform_nodereference_count}");
  
  $tally = array();
  
  $result = db_query("SELECT * FROM {webform_nodereference}");
  while ( $ref = db_fetch_object($result) ) {
    if ( !isset($tally[$ref->rnid][$ref->nid][$ref->cid]) ) {
      $tally[$ref->rnid][$ref->nid][$ref->cid] = 1;
    }
    else {
      $tally[$ref->rnid][$ref->nid][$ref->cid]++;
    }
  }
  
  foreach ( $tally as $nid => $a ) {
    foreach ( $a as $wnid => $b ) {
      foreach ( $b as $cid => $c ) {
        db_query("INSERT INTO {webform_nodereference_count} (nid, wnid, cid, total) VALUES (%d, %d, %d, %d)", $nid, $wnid, $cid, $c);
      }
    }
  }
  
  drupal_set_message('Rebuild count index');
}