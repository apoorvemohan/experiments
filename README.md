# experiments
1. All the python scripts are in ieeeclusterproject/bin
2. All the rules are in ieeeclusterproject/common/imports/rules.py
3. You will need to compile dmtcp (code with my plugin present in ieeeclusterproject/dmtcp, plugin is ieeeclusterproject/dmtcp/src/plugin/myplug/myplug.c)
4. You will also need to edit the base paths in ieeeclusterproject/common/imports/constants.py
5. ieeeclusterproject/bin/job_creator <number of jobs> creates job in ieeeclusterproject/data/parallel_dmtcp/appdata/app_instance_xml by default. You can modify the path as you want by modifying the ieeeclusterproject/bin/job_creator script.
6. All valid apps from which jobs can be created can be found in ieeeclusterproject/data/app_instances and their related executable commands can be found in ieeeclusterproject/data/app_commands.py
7. I have modified ieeeclusterproject/bin/job_creator to use particular apps. 
