local target = entity_query:get_first_entity_by_name("target")
retrieved_script_value = target.script:get("script_global_value")
non_existent_script_value = target.script:get("non_existent_value")
target.script:set("script_another_value", "yes")
