
#cpu utilization + max thread first + overcommit
def rule8(running_history_map, stats_history_map, app_stats_map1, threads, runninglist, total_passes):
        import copy
        import rules_utils as ru
        import multiprocessing as m
        import xml.etree.ElementTree as ET
        import re
        import constants as c
        import math
	import random

        totalthreads = m.cpu_count()*1.25
        app_stats_map = copy.deepcopy(app_stats_map1)
        next_runnable = []

	################# APP MEMORY NOT CONSIDERED ###############

	#first time
	if total_passes == 0:
		while len(app_stats_map) > 0:
			#app = ru.get_app_with_max_threads(app_stats_map)
			app = random.choice(app_stats_map.keys())
			tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
			root = tree.getroot()
			thread = int(root.findall('THREADS')[0].text)
			if ((totalthreads - thread) >= 0):
				totalthreads -= thread
				next_runnable.append(app)
			app_stats_map.pop(app)

		print "FIRST TIME"
		return next_runnable

	#check if any app completed - if yes, start random jobs
	for app in running_history_map[total_passes]:
		if app not in app_stats_map.keys():
			while len(app_stats_map) > 0:
				#app = ru.get_app_with_max_threads(app_stats_map)
				app = random.choice(app_stats_map.keys())
				tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
				root = tree.getroot()
				thread = int(root.findall('THREADS')[0].text)
				if ((totalthreads - thread) >= 0):
					totalthreads -= thread
					next_runnable.append(app)
				app_stats_map.pop(app)
			print "Some App Completed"
			return next_runnable

	#find throughput of previous apps
	cur_ins = 0
	prev_ins = 0
	cur_cyc = 0
	prev_cyc = 0
	for app in running_history_map[total_passes]:
		cur_ins = cur_ins + stats_history_map[app]['INSTRUCTIONS'][len(stats_history_map[app]['INSTRUCTIONS']) - 1]
		cur_cyc = cur_cyc + stats_history_map[app]['CPU_CYCLES'][len(stats_history_map[app]['CPU_CYCLES']) - 1]
		prev_ins = prev_ins + stats_history_map[app]['INSTRUCTIONS'][0]
		prev_cyc = prev_cyc + stats_history_map[app]['CPU_CYCLES'][0]
	
	throughput = ((float(cur_ins)/float(cur_cyc)))/((float(prev_ins)/float(prev_cyc)))
	print 'Throughput: ' + str(throughput)

	if throughput > 1:
		return running_history_map[total_passes]
	else:
		while len(app_stats_map) > 0:
			#app = ru.get_app_with_max_threads(app_stats_map)
			app = random.choice(app_stats_map.keys())
			tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
			root = tree.getroot()
			thread = int(root.findall('THREADS')[0].text)
			if ((totalthreads - thread) >= 0):
				totalthreads -= thread
				next_runnable.append(app)
			app_stats_map.pop(app)

	return next_runnable

def rule10(running_history_map, stats_history_map, app_stats_map1, threads, runninglist, total_passes):
        import copy
        import rules_utils as ru
        import multiprocessing as m
        import xml.etree.ElementTree as ET
        import re
        import constants as c
        import math
	import random

	totalram = 0
	meminfo = open('/proc/meminfo').read()
	matched = re.search(r'^MemTotal:\s+(\d+)', meminfo)
	if matched: 
		totalram = int(matched.groups()[0])

	totalram = totalram*0.95

        totalthreads = m.cpu_count()*1.25
        app_stats_map = copy.deepcopy(app_stats_map1)
        next_runnable = []

	################# APP MEMORY NOT CONSIDERED ###############

	#first time
	if total_passes == 0:
		while len(app_stats_map) > 0:
			#app = ru.get_app_with_max_threads(app_stats_map)
			#app = random.choice(app_stats_map.keys())
			app = get_app_with_max_threads(app_stats_map.keys())
			tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
			root = tree.getroot()
			thread = int(root.findall('THREADS')[0].text)
			ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
			if ((totalthreads - thread) >= 0) and ((totalram - ram) > 0):
				totalthreads -= thread
				totalram -= ram
				next_runnable.append(app)
			app_stats_map.pop(app)

		print "FIRST TIME"
		return next_runnable

	#check if any app completed - if yes, start random jobs
	for app in running_history_map[total_passes]:
		if app not in app_stats_map.keys():
			while len(app_stats_map) > 0:
				#app = ru.get_app_with_max_threads(app_stats_map)
				#app = random.choice(app_stats_map.keys())
			        app = get_app_with_max_threads(app_stats_map.keys())
				tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
				root = tree.getroot()
				thread = int(root.findall('THREADS')[0].text)
				ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
				if ((totalthreads - thread) >= 0) and ((totalram - ram) > 0):
					totalthreads -= thread
					totalram -= ram
					next_runnable.append(app)
				app_stats_map.pop(app)
			print "Some App Completed"
			return next_runnable

	#find throughput of previous apps
	cur_ins = 0
	prev_ins = 0
	cur_cyc = 0
	prev_cyc = 0
	for app in running_history_map[total_passes]:
		cur_ins = cur_ins + stats_history_map[app]['INSTRUCTIONS'][len(stats_history_map[app]['INSTRUCTIONS']) - 1]
		cur_cyc = cur_cyc + stats_history_map[app]['CPU_CYCLES'][len(stats_history_map[app]['CPU_CYCLES']) - 1]
		prev_ins = prev_ins + stats_history_map[app]['INSTRUCTIONS'][0]
		prev_cyc = prev_cyc + stats_history_map[app]['CPU_CYCLES'][0]
	
	throughput = ((float(cur_ins)/float(cur_cyc)))/((float(prev_ins)/float(prev_cyc)))
	print 'Throughput: ' + str(throughput)

	if throughput > 1:
		return running_history_map[total_passes]
	else:
		while len(app_stats_map) > 0:
			#app = ru.get_app_with_max_threads(app_stats_map)
			#app = random.choice(app_stats_map.keys())
			app = get_app_with_max_threads(app_stats_map.keys())
			tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
			root = tree.getroot()
			thread = int(root.findall('THREADS')[0].text)
			ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
			if ((totalthreads - thread) >= 0) and ((totalram - ram) > 0):
				totalthreads -= thread
				totalram -= ram
				next_runnable.append(app)
			app_stats_map.pop(app)

	return next_runnable

#cpu utilization + max thread first + overcommit
def rule9(running_history_map, stats_histroy_map, app_stats_map, threads, runninglist, total_passes):

	import copy
	import rules_utils as ru
	import multiprocessing as m
	import xml.etree.ElementTree as ET
	import re
	import constants as c
	import math

	total_passes /= 2

	totalram = 0
	meminfo = open('/proc/meminfo').read()
	matched = re.search(r'^MemTotal:\s+(\d+)', meminfo)
	if matched: 
		totalram = int(matched.groups()[0])
	if threads == 0:
		totalthreads = ru.getcpuidleperc() * 1.25
	else:
		totalthreads = threads

	#totalthreads = ru.getcpuidleperc() 
	print 'TotalThreads to start: ' + str(totalthreads)
	app_stats_map = copy.deepcopy(app_stats_map)
	next_runnable = []
	toberemoved = None

	print 'totalram: ' + str(totalram)

	average = 0.0
	while app in app_stats_map.keys():
		average += app_stats_map[app]['pickup_count']

	average /= float(total_passes)
	average /= float(len(app_stats_map))
	keys = app_stats_map.keys()

	aged = []
	for app in keys:
		if float(app_stats_map[app]['pickup_count'])/float(total_passes) < average:
			aged.append(app)

	app_cm = {}
	for app in stats_histroy_map:
		app_cm[app] = float(sum(stats_history_map[app]['CACHE_MISSES']))/float(len(stats_history_map[app]['CACHE_MISSES']))

	avg_cm = float(sum(entry[1] for entry in app_cm))/float(len(app_cm))
	mincm = maxcm = 0

	#Aged
	for app in aged:
		if totalthreads > 0:
			tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
			root = tree.getroot()
			thread = int(root.findall('THREADS')[0].text)
			if (totalthreads - thread) >= 0:
				next_runnable.append(app)
				totalthreads -= thread
				app_stats_map.pop(app)	
				if app_cm[app] < avg_cm:
					mincm += thread
				else:
					maxcm += thread
				app_cm.pop(app)

	max_apps = min_apps = []
	for app in app_cm.keys():
		if app_cm[app] < avg_cm:
			min_apps.append(app)
		else:
			max_apps.append(app)

	#Special
	while (totalthreads > 0) and (len(app_stats_map) > 0):
		next_app = None
		next_app_type = None
		if maxcm < mincm:
			for app in max_apps:
				tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
				root = tree.getroot()
				thread = int(root.findall('THREADS')[0].text)
				if (totalthreads - thread) >= 0:
					totalthreads -= thread
					next_runnable.append(app)
					maxcm += thread
					next_app = app
					next_app_type = 'MAX'
					break
		else:
			for app in min_apps:
				tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
				root = tree.getroot()
				thread = int(root.findall('THREADS')[0].text)
				if (totalthreads - thread) >= 0:
					totalthreads -= thread
					next_runnable.append(app)
					maxcm += thread
					next_app = app
					next_app_type = 'MIN'
					break

		if next_app_type == 'MAX':
			max_apps.remove(next_app)
		elif next_app_type == 'MIN':
			min_apps.remove(next_app)

		app_stats_map.pop(app)

	return next_runnable

RULES = {'rule8': rule8, 'rule9' : rule9, 'rule10': rule10}
