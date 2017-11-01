CiviCRM.org News Feed Module
============================

This module aggregates rss feeds for the in-app news dashboard widget. It reads from two Views:

1. [News Widget Feed](https://civicrm.org/admin/structure/views/view/news_widget_feed/edit)
2. [Event Widget Feed](https://civicrm.org/admin/structure/views/view/event_widget_feed/edit)

Every feed in the View becomes a tab in the dashboard widget; title, description and content can be controlled entirely through editing the View.

If the feed returns no results, the tab will not show.

The feed is published at https://civicrm.org/news-feed.rss and is consumed by CiviCRM clients via [CRM_Dashlet_Page_Blog](https://github.com/civicrm/civicrm-core/blob/master/CRM/Dashlet/Page/Blog.php#L43).
