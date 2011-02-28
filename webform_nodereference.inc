<?php

/**
 * @file
 * Webform module Node Reference component.
 */

/**
 * Implements _webform_default_component().
 */
function _webform_defaults_nodereference() {
  return array(
    'name' => '',
    'form_key' => NULL,
    'pid' => 0,
    'weight' => 0,
    'value' => '',
    'mandatory' => 0,
    'extra' => array(
      'title_display' => 0,
      'description' => '',
      'attributes' => array(),
      'nodereference_node_types' => array(),
      'nodereference_node_types_view' => '',
      'nodereference_node_types_view_args' => '',
    ),
  );
}

/**
 * Implements _webform_theme_component().
 */
function _webform_theme_nodereference() {
  return array(
    'webform_display_nodereference' => array(
      'arguments' => array('element' => NULL),
    ),
  );
}

/**
 * Implements _webform_edit_component().
 */
function _webform_edit_nodereference($component) {
  $types = array();
  foreach ( node_get_types('types') as $type ) {
    $types[$type->type] = $type->name;
  }
  
  $form['extra']['nodereference_node_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Node types'),
    '#description' => t('Node types that are available to choose for node reference'),
    '#options' => $types,
    '#default_value' => $component['extra']['nodereference_node_types'],
  );
  
  if ( module_exists('views') ) {
    $views = array('--' => '--');
    $all_views = views_get_all_views();
    foreach ( $all_views as $view ) {
      if ( $view->base_table == 'node' && !empty($view->display['default']->display_options['fields']) ) {
        if ( $view->type == 'default' ) {
          $views[t('Default Views')][$view->name] = $view->name;
        }
        else {
          $views[t('Existing Views')][$view->name] = $view->name;
        }
      }
    }
    
    if ( count($views) > 0 ) {
      $form['extra']['nodereference_node_types_view'] = array(
        '#type' => 'select',
        '#title' => t('View used to select the nodes'),
        '#description' => t('<p>Choose the "Views module" view that selects the nodes that can be referenced.<br />Note:</p>') .
          t('<ul><li>Only views that have fields will work for this purpose.</li><li>This will discard the "Content types" settings above. Use the view\'s "filters" section instead.</li><li>Use the view\'s "fields" section to display additional informations about candidate nodes on node creation/edition form.</li><li>Use the view\'s "sort criteria" section to determine the order in which candidate nodes will be displayed.</li></ul>'),
        '#options' => $views,
        '#default_value' => isset($component['extra']['nodereference_node_types_view']) ? $component['extra']['nodereference_node_types_view'] : '--',
      );
      $form['extra']['nodereference_node_types_view_args'] = array(
        '#type' => 'textfield',
        '#title' => t('View arguments'),
        '#description' => t('Provide a comma separated list of arguments to pass to the view.'),
        '#default_value' => isset($component['extra']['nodereference_node_types_view_args']) ? $component['extra']['nodereference_node_types_view_args'] : '',
        '#required' => FALSE,
      );
    }
  }
  else {
    $form['extra']['no_view_help'] = array(
      '#value' => t('<p>The list of nodes that can be referenced can be based on a "Views module" view but no appropriate views were found. <br />Note:</p>') .
        t('<ul><li>Only views that have fields will work for this purpose.</li><li>This will discard the "Content types" settings above. Use the view\'s "filters" section instead.</li><li>Use the view\'s "fields" section to display additional informations about candidate nodes on node creation/edition form.</li><li>Use the view\'s "sort criteria" section to determine the order in which candidate nodes will be displayed.</li></ul>'),
    );
  }
  
  return $form;
}

/**
 * Implements _webform_render_component().
 */
function _webform_render_nodereference($component, $value = NULL, $filter = TRUE) {
  $nodes = _webform_nodereference_potential_references($component);
  
  if ( count($nodes) > 0 ) {
    $element = array(
      '#type' => 'radios',
      '#title' => $component['name'],
      '#required' => $component['mandatory'],
      '#weight' => $component['weight'],
      '#prefix' => '<div class="webform-component-nodereference" id="webform-component-' . $component['form_key'] .'">',
      '#suffix' => '</div>',
      '#webform_component' => $component,
      '#options' => $nodes,
      '#default_value' => $value[0],
    );
  }
  else {
    $element = array(
      '#type' => 'markup',
      '#title' => $component['name'],
      '#value' => t('No nodes found.'),
      '#weight' => $component['weight'],
      '#webform_component' => $component,
    );
  }
  
  return $element;
}

/**
 * Implements _webform_display_component().
 */
function _webform_display_nodereference($component, $value, $format = 'html') {
  return array(
    '#title' => $component['name'],
    '#weight' => $component['weight'],
    '#theme' => 'webform_display_nodereference',
    '#theme_wrappers' => 'html' ? array('webform_element', 'webform_element_wrapper') : array('webform_element_text'),
    '#post_render' => array('webform_element_wrapper'),
    '#format' => $format,
    '#value' => isset($value[0]) ? $value[0] : '',
    '#webform_component' => $component,
  );
}

/**
 * Implements _webform_analysis_component().
 */
function _webform_analysis_nodereference($component, $sids = array()) {
  $rows[0] = array(t('Blank'), 0);
  $rows[1] = array(t('Blank'), 0);
  $rows[2] = array(t('Blank'), 0);
  return $rows;
}

/**
 * Implements _webform_table_component().
 */
function _webform_table_nodereference($component, $value) {
  return check_plain(empty($value[0]) ? '' : $value[0]);
}

/**
 * Implements _webform_csv_headers_component().
 */
function _webform_csv_headers_nodereference($component, $export_options) {
  $header = array();
  $header[0] = '';
  $header[1] = '';
  $header[2] = $component['name'];
  return $header;
}

/**
 * Implements _webform_csv_data_component().
 */
function _webform_csv_data_nodereference($component, $export_options, $value) {
  return !isset($value[0]) ? '' : $value[0];
}

/**
 * Fetch an array of all candidate referenced nodes.
 */
function _webform_nodereference_potential_references($component) {
  $references = FALSE;
  
  // If a view is defined, use that for our reference mode.
  if ( module_exists('views') && !empty($component['extra']['nodereference_node_types_view']) && $component['extra']['nodereference_node_types_view'] != '--' ) {
    $references = _webform_nodereference_potential_references_views($component);
  }
  
  // If there are no references, default to standard mode.
  if ( !$references ) {
    $references = _webform_nodereference_potential_references_standard($component);
  }
  
  return !empty($references) ? $references : array();
}

/**
 * Helper function for _webform_nodereference_potential_references().
 */
function _webform_nodereference_potential_references_views($component) {
  $view_name = $component['extra']['nodereference_node_types_view'];
  
  if ( $view = views_get_view($view_name) ) {
    $display = $view->add_display('content_references');
    $view->set_display($display);
    
    $options = array(
      'table' => 'node',
      'field_string' => 'title',
      'field_id' => 'nid',
    );
    
    $view->display_handler->set_option('style_plugin', 'content_php_array_autocomplete');
    $view->display_handler->set_option('row_plugin', 'fields');
    $view->display_handler->set_option('content_title_field', 'title');
    $view->display_handler->set_option('content_options', $options);
    $view->display_handler->set_option('items_per_page', 0);
    
    // Get arguments for the view.
    if ( !empty($component['extra']['nodereference_node_types_view_args']) ) {
      $view_args = array_map('trim', explode(',', $component['extra']['nodereference_node_types_view_args']));
    }
    else {
      $view_args = array();
    }
    
    // Add title field.
    $fields = $view->get_items('field', $display);
    if ( !isset($fields['title']) ) {
      $view->add_item($display, 'field', 'node', 'title');
    }
    
    // If not set, make all fields inline and define a separator.
    $options = $view->display_handler->get_option('row_options');
    if (empty($options['inline'])) {
      $options['inline'] = drupal_map_assoc(array_keys($view->get_items('field', $display)));
    }
    if (empty($options['separator'])) {
      $options['separator'] = '-';
    }
    $view->display_handler->set_option('row_options', $options);
    
    // Make sure the query is not cached
    $view->is_cacheable = FALSE;

    // Get the results.
    $result = $view->execute_display($display, $view_args);
    
    if ( count($result) > 0 ) {
      foreach ( $result as $nid => $node ) {
        $references[$nid] = check_plain($node['title']);
      }
    }
  }
  else {
    $references = array();
  }
  
  return $references;
}

/**
 * Helper function for _webform_nodereference_potential_standard().
 */
function _webform_nodereference_potential_references_standard($component) {
  $related_types = array();
  $args = array();
  
  if ( is_array($component['extra']['nodereference_node_types']) ) {
    foreach ( $component['extra']['nodereference_node_types'] as $related_type ) {
      if ( $related_type != '0' ) {
        $related_types[] = "n.type = '%s'";
        $args[] = $related_type;
      }
    }
  }
  
  if ( count($related_types) ) {
    $where = implode(' OR ', $related_types);
  }
  
  $where_clause = isset($where) ? "WHERE (". $where .")" : '';
  $sql = db_rewrite_sql("SELECT n.nid, n.title, n.type FROM {node} n $where_clause");
  $result = db_query($sql, $args);
  
  $references = array();
  while ( $node = db_fetch_object($result) ) {
    $references[$node->nid] = check_plain($node->title);
  }
  
  return $references;
}

/**
 * Format the output of data for this component.
 */
function theme_webform_display_nodereference($element) {
  $value = $element['#format'] == 'html' ? check_plain($element['#value']) : $element['#value'];
  return $value !== '' ? ($value) : ' ';
}
