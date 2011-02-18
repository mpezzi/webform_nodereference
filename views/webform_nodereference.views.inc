<?php

/**
 * @file
 * Views hooks implemented for Webform Node Reference module.
 */

/**
 * Implements hook_views_data_alter().
 */
function webform_nodereference_views_data_alter(&$data) {
  $data['node']['webform_nodereference_count'] = array(
    'field' => array(
      'title' => t('Webform Node Reference count'),
      'help' => t('The number of webform nodereferences on this node.'),
      'handler' => 'webform_nodereference_handler_field_nodereference_count',
      'count_type' => 'node',
    ),
  );
}

/**
 * Implements hook_views_handlers().
 */
function webform_nodereference_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'webform_nodereference') . '/views',
    ),
    'handlers' => array(
      'webform_nodereference_handler_field_nodereference_count' => array(
        'parent' => 'views_handler_field',
        'file' => 'webform_nodereference_handler_field_nodereference_count.inc',
      ),
    ),
  );
}
