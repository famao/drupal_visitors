<?php

# TODO: Create constants for folders.
# TODO: Add visitors_get_current_page function.
# TODO: Move width and height to separate functions.
/**
 * @file
 * Admin page callbacks for the visitors module.
 */

require_once dirname(__FILE__) .'/visitors.chart';

require_once dirname(__FILE__) .'/forms/date_filter.php';
require_once dirname(__FILE__) .'/forms/referers.php';

require_once dirname(__FILE__) .'/reports/days_of_month.php';
require_once dirname(__FILE__) .'/reports/days_of_week.php';
require_once dirname(__FILE__) .'/reports/hit_details.php';
require_once dirname(__FILE__) .'/reports/hosts.php';
require_once dirname(__FILE__) .'/reports/hours.php';
require_once dirname(__FILE__) .'/reports/monthly_history.php';
require_once dirname(__FILE__) .'/reports/recent_hits.php';
require_once dirname(__FILE__) .'/reports/referers.php';
require_once dirname(__FILE__) .'/reports/top_pages.php';
require_once dirname(__FILE__) .'/reports/user_activity.php';

