<?php

/**
 * Field handler to present webform nodereference count.
 */
class webform_nodereference_handler_field_nodereference_count extends views_handler_field {
  function construct() {
    parent::construct();
    $this->count_type = $this->definition['count_type'];
    $this->additional_fields['nid'] = 'nid';
  }
  
  function option_definition() {
    $options = parent::option_definition();
    $options['label'] = array('default' => '# of References', 'translatable' => TRUE);
    $options['webform_nid'] = array('default' => 0);
    $options['webform_cid'] = array('default' => 0);
    return $options;
  }
  
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    
    $form['webform_nid'] = array(
      '#type' => 'textfield',
      '#title' => t('Webform Node'),
      '#default_value' => $this->options['webform_nid'],
    );
    
    $form['webform_cid'] = array(
      '#type' => 'textfield',
      '#title' => t('Webform Component ID'),
      '#default_value' => $this->options['webform_cid'],
    );
  }
  
  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }
  
  function render($values) {
    $output = 0;
    
    $node = node_load($values->{$this->aliases['nid']});
    if ( isset($node->webform_nodereference[$this->options['webform_nid']][$this->options['webform_cid']]) ) {
      $output = $node->webform_nodereference[$this->options['webform_nid']][$this->options['webform_cid']];
    }
    
    return $output;
  }
}
