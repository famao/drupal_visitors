<?php

/**
 * @file
 * Admin page callbacks for the visitors module.
 */

define('VISITORS_MODULE_DIR'        , dirname(__FILE__));
define('VISITORS_MODULE_FORMS_DIR'  , VISITORS_MODULE_DIR . '/forms');
define('VISITORS_MODULE_REPORTS_DIR', VISITORS_MODULE_DIR . '/reports');

require_once VISITORS_MODULE_FORMS_DIR . '/date_filter.inc';
require_once VISITORS_MODULE_FORMS_DIR . '/referers.inc';

require_once VISITORS_MODULE_REPORTS_DIR . '/cities.inc';
require_once VISITORS_MODULE_REPORTS_DIR . '/city_hits.inc';
require_once VISITORS_MODULE_REPORTS_DIR . '/countries.inc';
require_once VISITORS_MODULE_REPORTS_DIR . '/days_of_week.inc';
require_once VISITORS_MODULE_REPORTS_DIR . '/monthly_history.inc';
require_once VISITORS_MODULE_REPORTS_DIR . '/referers.inc';

