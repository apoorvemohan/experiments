import os
f1 = open('out.txt', 'w+')
a = []
for filename in os.listdir('.'):


   if (filename.find('luc') != -1) or (filename.find('radix') != -1) or (filename.find('fmm') != -1):

	write = filename + ' - '
	lines = open(filename, 'r').readlines()

#	total_instances = float(int(filename[filename.find('.', filename.find('.') + 1) + 1:filename.find('.', filename.find('.', filename.find('.') + 1) + 1)]))

	cm = 0.0;pf = 0.0;tm = 0.0;ltm = 0.0

	for line in lines:
		if line.find('cache-misses') != -1:
			cm += float(line.strip().split(' ')[0].replace(',', ''))
	
	write += ('total-cache-misses: ' + str(int(cm)) + " ")

        for line in lines:
                if line.find('page-faults') != -1:
                        pf += float(line.strip().split(' ')[0].replace(',', ''))

        write += ('total-page-faults: ' + str(int(pf)) + " ")

        for line in lines:
                if line.find('seconds time elapsed') != -1:
                        tm += float(line.strip().split(' ')[0].replace(',', ''))
			ltm = float(line.strip().split(' ')[0].replace(',', ''))


	if filename.find('.p.') != -1:
		tm = ltm

        write += ('total-elapsed-time: ' + str(tm) + " \n")
	a.append(write)

a.sort()
for ele in a:
	f1.write(ele)
f1.close()
