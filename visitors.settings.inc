<?php

// $id:$//

/**
 * @file
 * Settings page callback file for the visitors module.
 */

/**
 * Menu callback;
 */
 
function visitors_admin_settings() {
  $form = array();
  // only administrators can access this function
  
  // Generate the form - settings applying to all patterns first
  $form['visitors_settings'] = array(
    '#type' => 'fieldset',
    '#weight' => -30,
    '#title' => t('Visitors block'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => t('Visitors block settings')
  );
  
  $form['visitors_settings']['visitors_show_total_visitors'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show Total Visitors'),
    '#default_value' => variable_get('visitors_show_total_visitors', 1),
    '#description' => t('Show Total Visitors')
  );    
    
  $form['visitors_settings']['visitors_show_unique_visitor'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show Unique Visitors'),
    '#default_value' => variable_get('visitors_show_unique_visitor', 1),
    '#description' => t('Show Unique Visitors based on their IP')
  );
  
  $form['visitors_settings']['visitors_registered_user'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show Registered Users'),
    '#default_value' => variable_get('visitors_registered_user', 1),
    '#description' => t('Show Registered Users')
  );
  
  $form['visitors_settings']['visitors_last_registered_user'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show Last Registered User'),
    '#default_value' => variable_get('visitors_last_registered_user', 1),
    '#description' => t('Show Last Registered User')
  );
  
  $form['visitors_settings']['visitors_published_nodes'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show Published Nodes'),
    '#default_value' => variable_get('visitors_published_nodes', 1),
    '#description' => t('Show Published Nodes')
  );
  
  $form['visitors_settings']['visitors_user_ip'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show User IP'),
    '#default_value' => variable_get('visitors_user_ip', 1),
    '#description' => t('Show User IP')
  );
  
  $form['visitors_settings']['visitors_since_date'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show Since Date'),
    '#default_value' => variable_get('visitors_since_date', 1),
    '#description' => t('Show Since Date')
  );
  
   return system_settings_form($form);
}
