; To build this file, use `drush make crm.make --no-core . --contrib-destination=.` from within the crm module directory

core = "6.x"
api = 2


projects[auto_nodetitle][version] = "1.2"
projects[auto_nodetitle][type] = "module"
projects[auto_nodetitle][subdir] = "contrib"

projects[dialog][type] = "module"
projects[dialog][subdir] = "contrib"

projects[gmap][version] = "1.1"
projects[gmap][type] = "module"
projects[gmap][subdir] = "contrib"

projects[location][version] = "3.1"
projects[location][type] = "module"
projects[location][subdir] = "contrib"

projects[nodereferrer][version] = "1.0-rc2"
projects[nodereferrer][type] = "module"
projects[nodereferrer][subdir] = "contrib"

projects[reverse_node_reference] = 1.0
projects[reverse_node_reference][type] = "module"
projects[reverse_node_reference][subdir] = "contrib"

projects[uc_views] = 3.0
projects[uc_views][type] = "module"
projects[uc_views][subdir] = "contrib"

projects[jquery_update] = 1.1
projects[jquery_update][type] = "module"
projects[jquery_update][subdir] = "contrib"

projects[views_bonus] = 1.1
projects[views_bonus][type] = "module"
projects[views_bonus][subdir] = "contrib"

projects[views_bulk_operations] = 1.9
projects[views_bulk_operations][type] = "module"
projects[views_bulk_operations][subdir] = "contrib"

projects[views_dynamic_fields] = 1.2
projects[views_dynamic_fields][type] = "module"
projects[views_dynamic_fields][subdir] = "contrib"


; Features
projects[crm_features][type] = "module"
projects[crm_features][download][type] = "git"
projects[crm_features][download][url] = "git://github.com/shomeya/crm_features.git"


; Custom modules
projects[multigroup_default_order][type] = "module"
projects[multigroup_default_order][subdir] = "custom"
projects[multigroup_default_order][download][type] = "git"
projects[multigroup_default_order][download][url] = "git://github.com/shomeya/multigroup_default_order.git"

projects[views_savedsearches][type] = "module"
projects[views_savedsearches][subdir] = "custom"
projects[views_savedsearches][download][type] = "git"
projects[views_savedsearches][download][url] = "git://github.com/shomeya/views_savedsearches.git"
