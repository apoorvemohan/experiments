
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

	totalthreads = 0
	if threads != None:
		totalthreads = threads
	else:
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

	totalthreads = 0
	if threads != None:
		totalthreads = threads
	else:
		totalthreads = m.cpu_count()*1

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
			ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
			if ((totalthreads - thread) >= 0) and ((totalram - ram) > 0):
				totalthreads -= thread
				totalram -= ram
				next_runnable.append(app)
			app_stats_map.pop(app)

		return next_runnable

	if threads == None:
		#check if any app completed - if yes, start random jobs
	#	for app in running_history_map[total_passes]:
#			if app not in app_stats_map.keys():
				while len(app_stats_map) > 0:
					#app = ru.get_app_with_max_threads(app_stats_map)
					app = random.choice(app_stats_map.keys())
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
	else:
		for app in running_history_map[total_passes]:
			if app in app_stats_map.keys():
				app_stats_map.pop(app)
		while len(app_stats_map) > 0:
			#app = ru.get_app_with_max_threads(app_stats_map)
			app = random.choice(app_stats_map.keys())
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

	if throughput > 0.95:
		return running_history_map[total_passes]
	else:
		while len(app_stats_map) > 0:
			#app = ru.get_app_with_max_threads(app_stats_map)
			app = random.choice(app_stats_map.keys())
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
	if threads == None:
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

#hyperthreading
def rule11(running_history_map, stats_history_map, app_stats_map1, threads, runninglist, total_passes):
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

	totalthreads = 0
	if threads > 0:
		totalthreads = threads
	else:
		totalthreads = (m.cpu_count()/2)*1.25

        app_stats_map = copy.deepcopy(app_stats_map1)
        next_runnable = [[],[]]

	################# APP MEMORY NOT CONSIDERED ###############

	#first time
	if total_passes == 0:
		while len(app_stats_map) > 0:
			#app = ru.get_app_with_max_threads(app_stats_map)
			app = random.choice(app_stats_map.keys())
			tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
			root = tree.getroot()
			thread = int(root.findall('THREADS')[0].text)
			ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
			if ((totalthreads - thread) >= 0) and ((totalram - ram) > 0):
				totalthreads -= thread
				totalram -= ram
				next_runnable[0].append(app)
			app_stats_map.pop(app)

		return next_runnable

	if threads == None:
		#check if any app completed - if yes, start random jobs
	#	for app in running_history_map[total_passes][0]:
	#		if app in app_stats_map.keys():
				while len(app_stats_map) > 0:
					#app = ru.get_app_with_max_threads(app_stats_map)
					app = random.choice(app_stats_map.keys())
					tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
					root = tree.getroot()
					thread = int(root.findall('THREADS')[0].text)
					ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
					if ((totalthreads - thread) >= 0) and ((totalram - ram) > 0):
						totalthreads -= thread
						totalram -= ram
						next_runnable[0].append(app)
					app_stats_map.pop(app)
				print "Some App Complete after midway"
				return next_runnable
	elif threads > 0:
		for app in running_history_map[total_passes][0]:
			if app in app_stats_map.keys():
				app_stats_map.pop(app)
		while len(app_stats_map) > 0:
			#app = ru.get_app_with_max_threads(app_stats_map)
			app = random.choice(app_stats_map.keys())
			tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
			root = tree.getroot()
			thread = int(root.findall('THREADS')[0].text)
			ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
			if ((totalthreads - thread) >= 0) and ((totalram - ram) > 0):
				totalthreads -= thread
				totalram -= ram
				next_runnable[0].append(app)
			app_stats_map.pop(app)
		print "Some App Completed before midway"
		return next_runnable

	print "No app completed"

	#find throughput of previous apps
	cur_ins = 0
	prev_ins = 0
	cur_cyc = 0
	prev_cyc = 0
	app_performing_bad = []
	app_performing_well = []
	for app in running_history_map[total_passes][0]:
		cur_ins = cur_ins + stats_history_map[app]['INSTRUCTIONS'][len(stats_history_map[app]['INSTRUCTIONS']) - 1]
		cur_cyc = cur_cyc + stats_history_map[app]['CPU_CYCLES'][len(stats_history_map[app]['CPU_CYCLES']) - 1]
		prev_ins = prev_ins + stats_history_map[app]['INSTRUCTIONS'][0]
		prev_cyc = prev_cyc + stats_history_map[app]['CPU_CYCLES'][0]
		if ((float(cur_ins)/float(cur_cyc))) < (((float(prev_ins)/float(prev_cyc)))):
			app_performing_bad.append([app, (float(cur_ins)/float(cur_cyc))])
		else:
			app_performing_well.append([app, (float(cur_ins)/float(cur_cyc))])
	
	throughput = ((float(cur_ins)/float(cur_cyc)))/((float(prev_ins)/float(prev_cyc)))
	print 'Throughput: ' + str(throughput)

	if throughput > 1:
		next_runnable[0] = copy.deepcopy(running_history_map[total_passes][0])
		next_runnable[1] = copy.deepcopy(running_history_map[total_passes][1])
		for app in list(set(next_runnable[0]) - set(next_runnable[1])):
			tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
			root = tree.getroot()
			app_core_req = int(root.findall('THREADS')[0].text)
			if (app in app_performing_bad) and (app not in running_history_map[total_passes][1]) and (app_core_req > 1):
				next_runnable[1].append(app)
				break

		return next_runnable

	elif len(running_history_map[total_passes][1]) > 0:
		next_runnable[0] = running_history_map[total_passes][0]
		next_runnable[1] = running_history_map[total_passes][1][:-1]
		return next_runnable
	else:
		while len(app_stats_map) > 0:
			#app = ru.get_app_with_max_threads(app_stats_map)
			app = random.choice(app_stats_map.keys())
			tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
			root = tree.getroot()
			thread = int(root.findall('THREADS')[0].text)
			ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
			if ((totalthreads - thread) >= 0) and ((totalram - ram) > 0):
				totalthreads -= thread
				totalram -= ram
				next_runnable[0].append(app)
			app_stats_map.pop(app)

	return next_runnable

#variable overcommit
def rule12(running_history_map, stats_history_map, app_stats_map1, threads, runninglist, total_passes):
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

	totalthreads = 0
	if threads > 0:
		totalthreads = threads
	else:
		totalthreads = m.cpu_count()

	max_limit = 2*totalthreads	

        app_stats_map = copy.deepcopy(app_stats_map1)
        next_runnable = [[],[]]

	################# APP MEMORY NOT CONSIDERED ###############

	#first time
	if total_passes == 0:
		while len(app_stats_map) > 0:
			#app = ru.get_app_with_max_threads(app_stats_map)
			app = random.choice(app_stats_map.keys())
			tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
			root = tree.getroot()
			thread = int(root.findall('THREADS')[0].text)
			ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
			if ((totalthreads - thread) >= 0) and ((totalram - ram) > 0):
				totalthreads -= thread
				totalram -= ram
				next_runnable[0].append(app)
			app_stats_map.pop(app)

		return next_runnable

	if threads == None:
		#check if any app completed - if yes, start random jobs
	#	for app in running_history_map[total_passes][0]:
	#		if app in app_stats_map.keys():
				while len(app_stats_map) > 0:
					#app = ru.get_app_with_max_threads(app_stats_map)
					app = random.choice(app_stats_map.keys())
					tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
					root = tree.getroot()
					thread = int(root.findall('THREADS')[0].text)
					ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
					if ((totalthreads - thread) >= 0) and ((totalram - ram) > 0):
						totalthreads -= thread
						totalram -= ram
						next_runnable[0].append(app)
					app_stats_map.pop(app)
				return next_runnable
	elif threads > 0:
		for app in running_history_map[total_passes][0]:
			if app in app_stats_map.keys():
				tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
				root = tree.getroot()
				totalram -= int(app_stats_map[app]['VmRSS'].split(' ')[0])
				app_stats_map.pop(app)

		print app_stats_map
		print totalram
		print totalthreads

		while len(app_stats_map) > 0:
			#app = ru.get_app_with_max_threads(app_stats_map)
			app = random.choice(app_stats_map.keys())
			tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
			root = tree.getroot()
			thread = int(root.findall('THREADS')[0].text)
			ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
			if ((totalthreads - thread) >= 0) and ((totalram - ram) > 0):
				totalthreads -= thread
				totalram -= ram
				next_runnable[0].append(app)
			app_stats_map.pop(app)
		print "Some App Completed before midway"
		return next_runnable

	print "No app completed"

	#find throughput of previous apps
	cur_ins = 0
	prev_ins = 0
	cur_cyc = 0
	prev_cyc = 0
	for app in running_history_map[total_passes][0]:
		cur_ins = cur_ins + stats_history_map[app]['INSTRUCTIONS'][len(stats_history_map[app]['INSTRUCTIONS']) - 1]
		cur_cyc = cur_cyc + stats_history_map[app]['CPU_CYCLES'][len(stats_history_map[app]['CPU_CYCLES']) - 1]
		prev_ins = prev_ins + stats_history_map[app]['INSTRUCTIONS'][0]
		prev_cyc = prev_cyc + stats_history_map[app]['CPU_CYCLES'][0]
	
	throughput = ((float(cur_ins)/float(cur_cyc)))/((float(prev_ins)/float(prev_cyc)))	
	print 'Throughput: ' + str(throughput)

	if throughput > 0.95:
		next_runnable[0] = copy.deepcopy(running_history_map[total_passes][0])
		next_runnable[1] = copy.deepcopy(running_history_map[total_passes][1])
		#add one app with min threads within the limit
		thread_sum = 0
		ram_sum = 0
		for app in next_runnable[0]:
			tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
			root = tree.getroot()
			thread_sum += int(root.findall('THREADS')[0].text)
			ram_sum += int(app_stats_map[app]['VmRSS'].split(' ')[0])

		min_thread_app = None
		probables_list = list(set(app_stats_map.keys()) - set(next_runnable[0]))

		while len(probables_list) > 0:
			app =  random.choice(probables_list)
			tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
			root = tree.getroot()
			app_core_req = int(root.findall('THREADS')[0].text)
			app_ram_req = int(app_stats_map[app]['VmRSS'].split(' ')[0])
			if ((thread_sum + app_core_req) <= max_limit) and ((ram_sum + app_ram_req) <= totalram):
				min_thread_app = app
				break
			probables_list.remove(app)
				
		if min_thread_app != None:
			next_runnable[1].append(min_thread_app)
			next_runnable[0].append(min_thread_app)
		else:
			print 'No new app could be appended to this group'

		return next_runnable

	elif len(running_history_map[total_passes][1]) > 0:
		next_runnable[0] = running_history_map[total_passes][0][:-1]
		next_runnable[1] = running_history_map[total_passes][1][:-1]
		return next_runnable
	else:
		while len(app_stats_map) > 0:
			#app = ru.get_app_with_max_threads(app_stats_map)
			app = random.choice(app_stats_map.keys())
			tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
			root = tree.getroot()
			thread = int(root.findall('THREADS')[0].text)
			ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
			if ((totalthreads - thread) >= 0) and ((totalram - ram) > 0):
				totalthreads -= thread
				totalram -= ram
				next_runnable[0].append(app)
			app_stats_map.pop(app)

	return next_runnable

#variable overcommit + normalization
def rule14(running_history_map, stats_history_map, app_stats_map1, threads, runninglist, total_passes):
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

	totalthreads = 0
	if threads > 0:
		totalthreads = threads
	else:
		totalthreads = m.cpu_count()

	max_limit = 20*totalthreads	

        app_stats_map = copy.deepcopy(app_stats_map1)
        next_runnable = [[],[]]

	################# APP MEMORY NOT CONSIDERED ###############

	#first time
	if total_passes == 0:
		while len(app_stats_map) > 0:
			#app = ru.get_app_with_max_threads(app_stats_map)
			app = random.choice(app_stats_map.keys())
			tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
			root = tree.getroot()
			thread = int(root.findall('THREADS')[0].text)
			ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
			if ((totalthreads - thread) >= 0) and ((totalram - ram) > 0):
				totalthreads -= thread
				totalram -= ram
				next_runnable[0].append(app)
			app_stats_map.pop(app)

		return next_runnable

	if threads == None:
		#check if any app completed - if yes, start random jobs
#		for app in running_history_map[total_passes][0]:
#			if app in app_stats_map.keys():
				while len(app_stats_map) > 0:
					#app = ru.get_app_with_max_threads(app_stats_map)
					app = random.choice(app_stats_map.keys())
					tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
					root = tree.getroot()
					thread = int(root.findall('THREADS')[0].text)
					ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
					if ((totalthreads - thread) >= 0) and ((totalram - ram) > 0):
						totalthreads -= thread
						totalram -= ram
						next_runnable[0].append(app)
					app_stats_map.pop(app)
				print "Some App Complete after midway"
				return next_runnable
	elif threads > 0:
		for app in running_history_map[total_passes][0]:
			if app in app_stats_map.keys():
				tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
				root = tree.getroot()
				totalram -= int(app_stats_map[app]['VmRSS'].split(' ')[0])
				app_stats_map.pop(app)

		while len(app_stats_map) > 0:
			#app = ru.get_app_with_max_threads(app_stats_map)
			app = random.choice(app_stats_map.keys())
			tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
			root = tree.getroot()
			thread = int(root.findall('THREADS')[0].text)
			ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
			if ((totalthreads - thread) >= 0) and ((totalram - ram) > 0):
				totalthreads -= thread
				totalram -= ram
				next_runnable[0].append(app)
			app_stats_map.pop(app)
		print "Some App Completed before midway"
		return next_runnable

	print "No app completed"

	#find throughput of previous apps
	deg_ratio = 0.95
	thread_sum = 0
	for app in running_history_map[total_passes][0]:
		tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
		root = tree.getroot()
		thread_sum += int(root.findall('THREADS')[0].text)
	if thread_sum > totalthreads:
		deg_ratio += (((thread_sum - totalthreads)/100)*5)
	elif thread_sum < totalthreads:
		deg_ratio = 0.95
	
	cur_ins = 0
	prev_ins = 0
	cur_cyc = 0
	prev_cyc = 0
	for app in running_history_map[total_passes][0]:
		cur_ins = cur_ins + stats_history_map[app]['INSTRUCTIONS'][len(stats_history_map[app]['INSTRUCTIONS']) - 1]
		cur_cyc = cur_cyc + stats_history_map[app]['CPU_CYCLES'][len(stats_history_map[app]['CPU_CYCLES']) - 1]
		prev_ins = prev_ins + stats_history_map[app]['INSTRUCTIONS'][0]
		prev_cyc = prev_cyc + stats_history_map[app]['CPU_CYCLES'][0]

	if thread_sum > totalthreads:
		throughput = ((float(cur_ins)/float(cur_cyc)))/(((float(prev_ins)/float(prev_cyc)))/(float(thread_sum)/float(totalthreads)))
	else:
		throughput = ((float(cur_ins)/float(cur_cyc)))/((float(prev_ins)/float(prev_cyc)))

	print 'Throughput: ' + str(throughput)

	if throughput > deg_ratio:
		next_runnable[0] = copy.deepcopy(running_history_map[total_passes][0])
		next_runnable[1] = copy.deepcopy(running_history_map[total_passes][1])
		#add one app with min threads within the limit
		thread_sum = 0
		ram_sum = 0
		for app in next_runnable[0]:
			tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
			root = tree.getroot()
			thread_sum += int(root.findall('THREADS')[0].text)
			ram_sum += int(app_stats_map[app]['VmRSS'].split(' ')[0])

		min_thread_app = None
		probables_list = list(set(app_stats_map.keys()) - set(next_runnable[0]))

		while len(probables_list) > 0:
			app =  random.choice(probables_list)
			tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
			root = tree.getroot()
			app_core_req = int(root.findall('THREADS')[0].text)
			app_ram_req = int(app_stats_map[app]['VmRSS'].split(' ')[0])
			if ((thread_sum + app_core_req) <= max_limit) and ((ram_sum + app_ram_req) <= totalram):
				min_thread_app = app
				break
			probables_list.remove(app)
				
		if min_thread_app != None:
			next_runnable[1].append(min_thread_app)
			next_runnable[0].append(min_thread_app)
		else:
			print 'No new app could be appended to this group'

		return next_runnable

	elif len(running_history_map[total_passes][1]) > 0:
		next_runnable[0] = running_history_map[total_passes][0][:-1]
		next_runnable[1] = running_history_map[total_passes][1][:-1]
		return next_runnable
	else:
		while len(app_stats_map) > 0:
			#app = ru.get_app_with_max_threads(app_stats_map)
			app = random.choice(app_stats_map.keys())
			tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
			root = tree.getroot()
			thread = int(root.findall('THREADS')[0].text)
			ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
			if ((totalthreads - thread) >= 0) and ((totalram - ram) > 0):
				totalthreads -= thread
				totalram -= ram
				next_runnable[0].append(app)
			app_stats_map.pop(app)

	return next_runnable

#no overcommit + degradation ratio 0.95 - no normalization
def rule15(running_history_map, stats_history_map, app_stats_map1, threads, runninglist, total_passes):
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

	totalthreads = 0
	if threads > 0:
		totalthreads = threads
	else:
		totalthreads = m.cpu_count()

	max_limit = totalthreads	

        app_stats_map = copy.deepcopy(app_stats_map1)
        next_runnable = [[],[]]

	################# APP MEMORY NOT CONSIDERED ###############

	#first time
	if total_passes == 0:
		while len(app_stats_map) > 0:
			#app = ru.get_app_with_max_threads(app_stats_map)
			app = random.choice(app_stats_map.keys())
			tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
			root = tree.getroot()
			thread = int(root.findall('THREADS')[0].text)
			ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
			if ((totalthreads - thread) >= 0) and ((totalram - ram) > 0):
				totalthreads -= thread
				totalram -= ram
				next_runnable[0].append(app)
			app_stats_map.pop(app)

		return next_runnable

	if threads == None:
		#check if any app completed - if yes, start random jobs
#		for app in running_history_map[total_passes][0]:
#			if app in app_stats_map.keys():
				while len(app_stats_map) > 0:
					#app = ru.get_app_with_max_threads(app_stats_map)
					app = random.choice(app_stats_map.keys())
					tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
					root = tree.getroot()
					thread = int(root.findall('THREADS')[0].text)
					ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
					if ((totalthreads - thread) >= 0) and ((totalram - ram) > 0):
						totalthreads -= thread
						totalram -= ram
						next_runnable[0].append(app)
					app_stats_map.pop(app)
				print "Some App Complete after midway"
				return next_runnable
	elif threads > 0:
		for app in running_history_map[total_passes][0]:
			if app in app_stats_map.keys():
				tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
				root = tree.getroot()
				totalram -= int(app_stats_map[app]['VmRSS'].split(' ')[0])
				app_stats_map.pop(app)

		while len(app_stats_map) > 0:
			#app = ru.get_app_with_max_threads(app_stats_map)
			app = random.choice(app_stats_map.keys())
			tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
			root = tree.getroot()
			thread = int(root.findall('THREADS')[0].text)
			ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
			if ((totalthreads - thread) >= 0) and ((totalram - ram) > 0):
				totalthreads -= thread
				totalram -= ram
				next_runnable[0].append(app)
			app_stats_map.pop(app)
		print "Some App Completed before midway"
		return next_runnable

	print "No app completed"

	#find throughput of previous apps
	cur_ins = 0
	prev_ins = 0
	cur_cyc = 0
	prev_cyc = 0
	for app in running_history_map[total_passes][0]:
		cur_ins = cur_ins + stats_history_map[app]['INSTRUCTIONS'][len(stats_history_map[app]['INSTRUCTIONS']) - 1]
		cur_cyc = cur_cyc + stats_history_map[app]['CPU_CYCLES'][len(stats_history_map[app]['CPU_CYCLES']) - 1]
		prev_ins = prev_ins + stats_history_map[app]['INSTRUCTIONS'][0]
		prev_cyc = prev_cyc + stats_history_map[app]['CPU_CYCLES'][0]
	
	throughput = ((float(cur_ins)/float(cur_cyc)))/((float(prev_ins)/float(prev_cyc)))	
	print 'Throughput: ' + str(throughput)

	if throughput > 0.95:
		next_runnable[0] = copy.deepcopy(running_history_map[total_passes][0])
		next_runnable[1] = copy.deepcopy(running_history_map[total_passes][1])

		return next_runnable

	else:
		while len(app_stats_map) > 0:
			#app = ru.get_app_with_max_threads(app_stats_map)
			app = random.choice(app_stats_map.keys())
			tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
			root = tree.getroot()
			thread = int(root.findall('THREADS')[0].text)
			ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
			if ((totalthreads - thread) >= 0) and ((totalram - ram) > 0):
				totalthreads -= thread
				totalram -= ram
				next_runnable[0].append(app)
			app_stats_map.pop(app)

	return next_runnable

#variable overcommit + normalization + if job finishes in middle, retry
def rule17(running_history_map, stats_history_map, app_stats_map1, threads, runninglist, total_passes):
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

	totalthreads = 0
	if threads > 0:
		totalthreads = threads
	else:
		totalthreads = m.cpu_count()

	max_limit = 20*totalthreads	

        app_stats_map = copy.deepcopy(app_stats_map1)
        next_runnable = [[],[],[],[]]

	################# APP MEMORY NOT CONSIDERED ###############

	#first time
	if total_passes == 0:
		while len(app_stats_map) > 0:
			#app = ru.get_app_with_max_threads(app_stats_map)
			app = random.choice(app_stats_map.keys())
			tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
			root = tree.getroot()
			thread = int(root.findall('THREADS')[0].text)
			ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
			if ((totalthreads - thread) >= 0) and ((totalram - ram) > 0):
				totalthreads -= thread
				totalram -= ram
				next_runnable[0].append(app)
			app_stats_map.pop(app)

		return next_runnable

	if threads == None:
		deg_ratio = 0.95
		thread_sum = 0
		for app in running_history_map[total_passes][0]:
			tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
			root = tree.getroot()
			thread_sum += int(root.findall('THREADS')[0].text)
		if thread_sum > totalthreads:
			deg_ratio += (((thread_sum - totalthreads)/100)*6)
		elif thread_sum < totalthreads:
			deg_ratio = 0.95
		
		cur_ins = 0
		prev_ins = 0
		cur_cyc = 0
		prev_cyc = 0
		for app in running_history_map[total_passes][0]:
			cur_ins = cur_ins + stats_history_map[app]['INSTRUCTIONS'][len(stats_history_map[app]['INSTRUCTIONS']) - 1]
			cur_cyc = cur_cyc + stats_history_map[app]['CPU_CYCLES'][len(stats_history_map[app]['CPU_CYCLES']) - 1]
			prev_ins = prev_ins + stats_history_map[app]['INSTRUCTIONS'][0]
			prev_cyc = prev_cyc + stats_history_map[app]['CPU_CYCLES'][0]

		if thread_sum > totalthreads:
			throughput = ((float(cur_ins)/float(cur_cyc)))/(((float(prev_ins)/float(prev_cyc)))/(float(thread_sum)/float(totalthreads)))
		else:
			throughput = ((float(cur_ins)/float(cur_cyc)))/((float(prev_ins)/float(prev_cyc)))

		print 'Throughput: ' + str(throughput)

		if throughput > deg_ratio:
	
			next_runnable[0] = copy.deepcopy(running_history_map[total_passes][0])
			next_runnable[1] = copy.deepcopy(running_history_map[total_passes][1])
			
			print next_runnable[0]
			print app_stats_map.keys()
	

			completed_apps = list(set(next_runnable[0]) - set(app_stats_map.keys()))
			for app in completed_apps:
					next_runnable[0].remove(app)

			completed_apps = list(set(next_runnable[1]) - set(app_stats_map.keys()))
			for app in completed_apps:
					next_runnable[1].remove(app)

			print next_runnable[0]
			print app_stats_map.keys()

			for app in next_runnable[0]:
				tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
				root = tree.getroot()
				totalthreads -= int(root.findall('THREADS')[0].text)
				totalram -= int(app_stats_map[app]['VmRSS'].split(' ')[0])

			if thread_sum >= totalthreads:
				return next_runnable
			else:
				next_runnable[1] = []
				while len(app_stats_map) > 0:
					app = random.choice(app_stats_map.keys())
					tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
					root = tree.getroot()
					thread = int(root.findall('THREADS')[0].text)
					ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
					if ((totalthreads - thread) >= 0) and ((totalram - ram) > 0):
						totalthreads -= thread
						totalram -= ram
						next_runnable[0].append(app)
					app_stats_map.pop(app)
				print "Some App Complete after midway"
				return next_runnable

		else:

			while len(app_stats_map) > 0:
				app = random.choice(app_stats_map.keys())
				tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
				root = tree.getroot()
				thread = int(root.findall('THREADS')[0].text)
				ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
				if ((totalthreads - thread) >= 0) and ((totalram - ram) > 0):
					totalthreads -= thread
					totalram -= ram
					next_runnable[0].append(app)
				app_stats_map.pop(app)
			print "Some App Complete after midway"
			return next_runnable

	if threads > 0:
		for app in running_history_map[total_passes][0]:
			if app in app_stats_map.keys():
				tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
				root = tree.getroot()
				totalram -= int(app_stats_map[app]['VmRSS'].split(' ')[0])
				app_stats_map.pop(app)

		while len(app_stats_map) > 0:
			#app = ru.get_app_with_max_threads(app_stats_map)
			app = random.choice(app_stats_map.keys())
			tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
			root = tree.getroot()
			thread = int(root.findall('THREADS')[0].text)
			ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
			if ((totalthreads - thread) >= 0) and ((totalram - ram) > 0):
				totalthreads -= thread
				totalram -= ram
				next_runnable[0].append(app)
			app_stats_map.pop(app)
		print "Some App Completed before midway"
		return next_runnable

	print "No app completed"

	#find throughput of previous apps
	deg_ratio = 0.95
	thread_sum = 0
	for app in running_history_map[total_passes][0]:
		tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
		root = tree.getroot()
		thread_sum += int(root.findall('THREADS')[0].text)
	if thread_sum > totalthreads:
		deg_ratio += (((thread_sum - totalthreads)/100)*6)
	elif thread_sum < totalthreads:
		deg_ratio = 0.95
	
	cur_ins = 0
	prev_ins = 0
	cur_cyc = 0
	prev_cyc = 0
	for app in running_history_map[total_passes][0]:
		cur_ins = cur_ins + stats_history_map[app]['INSTRUCTIONS'][len(stats_history_map[app]['INSTRUCTIONS']) - 1]
		cur_cyc = cur_cyc + stats_history_map[app]['CPU_CYCLES'][len(stats_history_map[app]['CPU_CYCLES']) - 1]
		prev_ins = prev_ins + stats_history_map[app]['INSTRUCTIONS'][0]
		prev_cyc = prev_cyc + stats_history_map[app]['CPU_CYCLES'][0]

	if thread_sum > totalthreads:
		throughput = ((float(cur_ins)/float(cur_cyc)))/(((float(prev_ins)/float(prev_cyc)))/(float(thread_sum)/float(totalthreads)))
	else:
		throughput = ((float(cur_ins)/float(cur_cyc)))/((float(prev_ins)/float(prev_cyc)))

	print 'Throughput: ' + str(throughput)

	if throughput > deg_ratio:
		next_runnable[0] = copy.deepcopy(running_history_map[total_passes][0])
		next_runnable[1] = copy.deepcopy(running_history_map[total_passes][1])
		#add one app with min threads within the limit
		thread_sum = 0
		ram_sum = 0
		for app in next_runnable[0]:
			tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
			root = tree.getroot()
			thread_sum += int(root.findall('THREADS')[0].text)
			ram_sum += int(app_stats_map[app]['VmRSS'].split(' ')[0])

		min_thread_app = None
		probables_list = list(set(app_stats_map.keys()) - set(next_runnable[0]))

		while len(probables_list) > 0:
			app =  random.choice(probables_list)
			tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
			root = tree.getroot()
			app_core_req = int(root.findall('THREADS')[0].text)
			app_ram_req = int(app_stats_map[app]['VmRSS'].split(' ')[0])
			if ((thread_sum + app_core_req) <= max_limit) and ((ram_sum + app_ram_req) <= totalram):
				min_thread_app = app
				break
			probables_list.remove(app)
				
		if min_thread_app != None:
			next_runnable[1].append(min_thread_app)
			next_runnable[0].append(min_thread_app)
		else:
			print 'No new app could be appended to this group'

		return next_runnable

	elif (len(running_history_map[total_passes][1]) > 0) and (len(running_history_map[total_passes][0]) > 1):
		next_runnable[0] = running_history_map[total_passes][0][:-1]
		next_runnable[1] = running_history_map[total_passes][1][:-1]
		return next_runnable
	else:
		while len(app_stats_map) > 0:
			#app = ru.get_app_with_max_threads(app_stats_map)
			app = random.choice(app_stats_map.keys())
			tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
			root = tree.getroot()
			thread = int(root.findall('THREADS')[0].text)
			ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
			if ((totalthreads - thread) >= 0) and ((totalram - ram) > 0):
				totalthreads -= thread
				totalram -= ram
				next_runnable[0].append(app)
			app_stats_map.pop(app)

	return next_runnable


RULES = {'rule8': rule8, 'rule9' : rule9, 'rule10': rule10, 'rule11': rule11, 'rule12': rule12, 'rule14': rule14, 'rule15': rule15, 'rule17': rule17}
