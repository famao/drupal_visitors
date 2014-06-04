<?php

/**
 * @file
 * Admin page callbacks for the visitors module.
 */

define('VISITORS_MODULE_DIR'        , dirname(__FILE__));
define('VISITORS_MODULE_FORMS_DIR'  , VISITORS_MODULE_DIR . '/forms');
define('VISITORS_MODULE_REPORTS_DIR', VISITORS_MODULE_DIR . '/reports');

require_once VISITORS_MODULE_FORMS_DIR . '/referers.inc';

require_once VISITORS_MODULE_REPORTS_DIR . '/referers.inc';

