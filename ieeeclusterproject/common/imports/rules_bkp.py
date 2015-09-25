#!/usr/bin/env python
"""
#overcommit threads by 2X - min cache misses
def rule1(stats_history_map, app_stats_map, thread, runninglist=None, total_passes=None):
	import copy
	import rules_utils as ru
	import multiprocessing as m
	import xml.etree.ElementTree as ET
	import re

	import constants as c

	totalram = 0

#	meminfo = open('/proc/meminfo').read()
#	matched = re.search(r'^MemTotal:\s+(\d+)', meminfo)
#	if matched: 
	#totalram = int(matched.groups()[0])

#	totalram = int(totalram * 0.85)

	app_stats_map = copy.deepcopy(app_stats_map)
	next_runnable = []
	totalthreads = m.cpu_count() * 2
	toberemoved = None

#	while((totalram > 0) and (totalthreads > 0) and ((len(app_stats_map)) > 0)):
	while len(app_stats_map) > 0:
		app = ru.get_app_with_min_cache_misses(app_stats_map)
		tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
		root = tree.getroot()
		thread = int(root.findall('THREADS')[0].text)
		ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
		if ((totalram - ram) > 0) and ((totalthreads - thread) >= 0):
			totalram -= ram
			totalthreads -= thread
			next_runnable.append(app)
#		distance = thread
		app_stats_map.pop(app)

	return next_runnable

'''	distance = 0
	if totalram < 0:
                for app in next_runnable:
                        if ((totalram + app[2]) < distance) and ((totalram + app[2]) >= 0):
                                distance = totalram + app[2]
                                toberemoved = next_runnable.index(app)

		totalthreads += next_runnable[toberemoved][1]
                next_runnable.pop(toberemoved)
	

	distance = 0

	if totalthreads < 0:
		for app in next_runnable:
			if ((totalthreads + app[1]) < distance) and ((totalthreads + app[1]) >= 0):
				distance = totalthreads + app[1]
				toberemoved = next_runnable.index(app)

		next_runnable.pop(toberemoved)
'''
#	return [app[0] for app in next_runnable]


#no overcommit - min cache misses
def rule2(stats_histroy_map, app_stats_map, runninglist, thread, total_passes=None):
        import copy
        import rules_utils as ru
        import multiprocessing as m
        import xml.etree.ElementTree as ET
        import re

        import constants as c

        totalram = 0

        meminfo = open('/proc/meminfo').read()
        matched = re.search(r'^MemTotal:\s+(\d+)', meminfo)
        if matched:
                totalram = int(matched.groups()[0])

        app_stats_map = copy.deepcopy(app_stats_map)
        next_runnable = []
        totalthreads = m.cpu_count()
        toberemoved = None

#       while((totalram > 0) and (totalthreads > 0) and ((len(app_stats_map)) > 0)):
        while len(app_stats_map) > 0:
                app = ru.get_app_with_min_cache_misses(app_stats_map)
                tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
                root = tree.getroot()
                thread = int(root.findall('THREADS')[0].text)
                ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
                if ((totalram - ram) > 0) and ((totalthreads - thread) >= 0):
                        totalram -= ram
                        totalthreads -= thread
                        next_runnable.append(app)
#               distance = thread
                app_stats_map.pop(app)

        return next_runnable

'''
#total app mem < total sys mem
def rule3(app_stats_map):
	print 'rule 3'

#total app mem < total sys mem and no overcommit threads
def rule4(app_stats_map):
	print 'rule4'

#total app mem < tota; sys mem and overcommit threads by 2X
def rule5(app_stats_map):
	print 'rule5'
'''

#2x overcommit + memory + minthreads firsts
def rule3(stats_histroy_map, app_stats_map, thread, runninglist=None):
	import copy
	import rules_utils as ru
	import multiprocessing as m
	import xml.etree.ElementTree as ET
	import re

	import constants as c

	totalram = 0

	meminfo = open('/proc/meminfo').read()
	matched = re.search(r'^MemTotal:\s+(\d+)', meminfo)
	if matched: 
		totalram = int(matched.groups()[0])

	app_stats_map = copy.deepcopy(app_stats_map)
	next_runnable = []
	totalthreads = m.cpu_count() * 2
	toberemoved = None

	while len(app_stats_map) > 0:
		app = ru.get_app_with_min_threads(app_stats_map)
		tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
		root = tree.getroot()
		thread = int(root.findall('THREADS')[0].text)
		ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
		if ((totalram - ram) > 0) and ((totalthreads - thread) >= 0):
			totalram -= ram
			totalthreads -= thread
			next_runnable.append(app)
		app_stats_map.pop(app)

	return next_runnable


#cpu utilization + min thread first
def rule4(stats_histroy_map, app_stats_map, runninglist, thread, total_passes=None):

	import copy
	import rules_utils as ru
	import multiprocessing as m
	import xml.etree.ElementTree as ET
	import re
	import constants as c
	import math

	totalram = 0
	meminfo = open('/proc/meminfo').read()
	matched = re.search(r'^MemTotal:\s+(\d+)', meminfo)
	if matched: 
		totalram = int(matched.groups()[0])

	totalthreads = math.ceil(ru.getcpuidleperc()/100)
	app_stats_map = copy.deepcopy(app_stats_map)
	next_runnable = []
	toberemoved = None

	if totalthreads > 0:
		while len(app_stats_map) > 0:
			app = ru.get_app_with_min_threads(app_stats_map)
			if not app in runninglist:
				tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
				root = tree.getroot()
				thread = int(root.findall('THREADS')[0].text)
				ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
				if ((totalram - ram) > 0) and ((totalthreads - thread) >= 0):
					totalram -= ram
					totalthreads -= thread
					next_runnable.append(app)
			app_stats_map.pop(app)

	return next_runnable

#min memory
def rule5(stats_histroy_map, app_stats_map, runninglist, thread, total_passes=None):

	import copy
	import rules_utils as ru
	import multiprocessing as m
	import xml.etree.ElementTree as ET
	import re
	import constants as c
	import math

	totalram = 0
	meminfo = open('/proc/meminfo').read()
	matched = re.search(r'^MemTotal:\s+(\d+)', meminfo)
	if matched: 
		totalram = int(matched.groups()[0])

	totalthreads = math.ceil(ru.getcpuidleperc()/100)
	app_stats_map = copy.deepcopy(app_stats_map)
	next_runnable = []
	toberemoved = None

	if totalthreads > 0:
		while len(app_stats_map) > 0:
			app = ru.get_app_with_min_memory(app_stats_map)
			if not app in runninglist:
				tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
				root = tree.getroot()
				thread = int(root.findall('THREADS')[0].text)
				ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
				if ((totalram - ram) > 0) and ((totalthreads - thread) >= 0):
					totalram -= ram
					totalthreads -= thread
					next_runnable.append(app)
			app_stats_map.pop(app)

	return next_runnable

#cpu utilization + max thread first + overcommit
def rule6(stats_histroy_map, app_stats_map, runninglist, thread, total_passes=None):

	import copy
	import rules_utils as ru
	import multiprocessing as m
	import xml.etree.ElementTree as ET
	import re
	import constants as c
	import math

	totalram = 0
	meminfo = open('/proc/meminfo').read()
	matched = re.search(r'^MemTotal:\s+(\d+)', meminfo)
	if matched: 
		totalram = int(matched.groups()[0])

	totalthreads = ru.getcpuidleperc() 
	print 'TotalThreads to start: ' + str(totalthreads)
	app_stats_map = copy.deepcopy(app_stats_map)
	next_runnable = []
	toberemoved = None

	print 'totalram: ' + str(totalram)

	if totalthreads > 0:
		while len(app_stats_map) > 0:
			app = ru.get_app_with_max_threads(app_stats_map)
			if not app in runninglist:
				tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
				root = tree.getroot()
				thread = int(root.findall('THREADS')[0].text)
				print 'app: ' + str(app)
				ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
				print 'ram: ' + str(ram)
				if ((totalram - ram) > 0) and ((totalthreads - thread) >= 0):
					totalram -= ram
					totalthreads -= thread
					next_runnable.append(app)
			app_stats_map.pop(app)

	next_runnable.extend(runninglist)
	return next_runnable


#cpu utilization + max thread first + overcommit
def rule7(stats_history_map, app_stats_map, runninglist, thread, total_passes=None):

   if len(app_stats_map) > 0:
        import copy
        import rules_utils as ru
        import multiprocessing as m
        import xml.etree.ElementTree as ET
        import re
        import constants as c
        import math

        totalram = 0
        meminfo = open('/proc/meminfo').read()
        matched = re.search(r'^MemTotal:\s+(\d+)', meminfo)
        if matched:
                totalram = int(matched.groups()[0])

	totalram = int(totalram * 0.9)

        totalthreads = ru.getcpuidleperc()
        print 'TotalThreads to start: ' + str(totalthreads)
        app_stats_map = copy.deepcopy(app_stats_map)
        next_runnable = []
        toberemoved = None

	runninglist.sort()
	app = ru.get_app_with_max_threads(app_stats_map)
	tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
	root = tree.getroot()
	thread = int(root.findall('THREADS')[0].text)
	if (len(runninglist) !=0) and (runninglist[0] < thread):
		runninglist = []
		totalthreads = m.cpu_count()
	print 'rule7'

        if totalthreads > 0:
                while len(app_stats_map) > 0:
                        app = ru.get_app_with_max_threads(app_stats_map)
                        if not app in runninglist:
                                tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
                                root = tree.getroot()
                                thread = int(root.findall('THREADS')[0].text)
                                print 'app: ' + str(app)
                                ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
                                print 'ram: ' + str(ram)
                                if ((totalram - ram) > 0) and ((totalthreads - thread) >= 0):
                                        totalram -= ram
                                        totalthreads -= thread
                                        next_runnable.append(app)
                        app_stats_map.pop(app)

        next_runnable.extend(runninglist)
        return next_runnable

   else:
	return []
"""

#cpu utilization + max thread first + overcommit
def rule8(stats_history_map, app_stats_map, runninglist, thread, total_passes=None):

   if len(app_stats_map) > 0:
        import copy
        import rules_utils as ru
        import multiprocessing as m
        import xml.etree.ElementTree as ET
        import re
        import constants as c
        import math

        totalthreads = m.cpu_count()*1.25
        app_stats_map = copy.deepcopy(app_stats_map)
        next_runnable = []
        toberemoved = None
	max_mem_app = -1
	max_mem = 0
	throughput = 0

	last_total_ins = 0
	last_total_cyc = 0
	current_total_ins = 0
	current_total_cyc = 0  

	if len(runninglist) > 0:
		#find running average
		for app in runninglist:
			#app_avg = sum(stats_history_map[app]['INSTRUCTIONS'])/len(stats_history_map[app]['INSTRUCTIONS'])
			last_total_ins = last_total_ins + (int(stats_history_map[app]['INSTRUCTIONS'][len(stats_history_map[app]['INSTRUCTIONS']) - 1])/1000000000)
			last_total_cyc = last_total_cyc + (int(stats_history_map[app]['CPU_CYCLES'][len(stats_history_map[app]['CPU_CYCLES']) - 1])/1000000000)
			current_total_ins = current_total_ins + (int(app_stats_map[app]['INSTRUCTIONS'])/1000000000)
			current_total_cyc = current_total_cyc + (int(app_stats_map[app]['CPU_CYCLES'])/1000000000)
			#throughput += app_avg/int(app_stats_map[app]['INSTRUCTIONS'])
			tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
			root = tree.getroot()
			ram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
			if ram > max_mem:
				max_mem = ram
				max_mem_app = app

		print 'curr ins: ' + str(current_total_ins)
		print 'curr cyc: ' + str(current_total_cyc)
		print 'last ins: ' + str(last_total_ins)
		print 'last cyc: ' + str(last_total_cyc)

		current_throughput = float(current_total_ins)/float(current_total_cyc)
		print 'current through: ' + str(current_throughput)
		last_throughput = float(last_total_ins)/float(last_total_cyc)
		print 'last through: ' + str(last_throughput)

		throughput = float(current_throughput)/float(last_throughput)

		#throughput /= len(runninglist)
		print 'Throughput: ' + str(throughput)

		if throughput < 0.95:
			runninglist.remove(max_mem_app)
			return runninglist
		else:
			#if totalthreads > 0:
			while len(app_stats_map) > 0:
				app = ru.get_app_with_max_threads(app_stats_map)
				if not app in runninglist:
					tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
					root = tree.getroot()
					thread = int(root.findall('THREADS')[0].text)
			#	if ((totalthreads - thread) >= 0):
			#		totalthreads -= thread
					next_runnable.append(app)
					break
				app_stats_map.pop(app)

			next_runnable.extend(runninglist)
			return next_runnable

		#else:
		#	return runninglist

        if totalthreads > 0:
                while len(app_stats_map) > 0:
                        app = ru.get_app_with_max_threads(app_stats_map)
                        if not app in runninglist:
                                tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
                                root = tree.getroot()
                                thread = int(root.findall('THREADS')[0].text)
                                if ((totalthreads - thread) >= 0):
                                        totalthreads -= thread
                                        next_runnable.append(app)
                        app_stats_map.pop(app)

		next_runnable.extend(runninglist)
		return next_runnable

   else:
	return []



#cpu utilization + max thread first + overcommit
def rule9(stats_histroy_map, app_stats_map, runninglist, total_passes, threads):

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


RULES = {'rule8': rule8, 'rule9' : rule9}
#RULES = {'rule1' : rule1, 'rule2' : rule2, 'rule3' : rule3, 'rule4' : rule4, 'rule6' : rule6, 'rule7' : rule7, 'rule8' : rule8, 'rule9' : rule9, 'rule10': rule10}
