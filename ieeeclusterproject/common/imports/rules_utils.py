#!/usr/bin/env python

def get_app_with_min_cache_misses(app_stats_map):
	minkey = app_stats_map.keys()[0]
	min_miss = int(app_stats_map[minkey]['CACHE_MISSES'])

	for app in app_stats_map.keys():
		if int(app_stats_map[app]['CACHE_MISSES']) < min_miss:
			minkey = app

	return minkey

def get_app_with_min_threads(app_stats_map):
	import constants as c	
	import xml.etree.ElementTree as ET

	minthreads = 1000000
	minthreadapp = None

	for app in app_stats_map:
		tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
		root = tree.getroot()
		thread = int(root.findall('THREADS')[0].text)
		if thread < minthreads:
			minthreads = thread
			minthreadapp = app

	return minthreadapp

def get_app_with_max_threads(app_stats_map):
	import constants as c	
	import xml.etree.ElementTree as ET

	maxthreads = -1
	maxthreadapp = None

	for app in app_stats_map:
		tree = ET.parse(c.PARALLEL_DMTCP_APP_INSTANCE_DIR + '/' + app + '.xml')
		root = tree.getroot()
		thread = int(root.findall('THREADS')[0].text)
		if thread > maxthreads:
			maxthreads = thread
			maxthreadapp = app

	return maxthreadapp

def getcpuidleperc():
	import subprocess as s
	import multiprocessing as m
	import math

	obj = s.Popen('top -n3 -d 0.5 | grep Cpu',shell=True, stdout=s.PIPE)
	obj.wait()
	string = obj.stdout.readlines()
	cpu = string[2].strip().split(',')[3].strip().split(' ')[1].strip()
	if(cpu == ''):
		cpu = string[2].strip().split(',')[3].strip().split(' ')[2].strip()

	cpuutilization = float(cpu.split('%')[0])
	cpus = m.cpu_count()

	totalthreads = (float(cpus) * cpuutilization) / 100.0

	print 'total: ' + str(totalthreads)

	if math.modf(totalthreads)[0] < 0.5:
		return int(totalthreads)
	else:
		return math.ceil(totalthreads)
	
def get_app_with_min_memory(app_stats_map):
	import re
	
	minram = 0
	minramapp = None

	meminfo = open('/proc/meminfo').read()
	matched = re.search(r'^MemTotal:\s+(\d+)', meminfo)
	if matched:  
		minram = int(matched.groups()[0])

	app_stats_map = copy.deepcopy(app_stats_map)

	while app in app_stats_map:
		if int(app_stats_map[app]['VmRSS'].split(' ')[0]) < minram:
			minram = int(app_stats_map[app]['VmRSS'].split(' ')[0])
			minramapp = app

	return minramapp 




