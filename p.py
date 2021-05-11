file = open('shit.c', 'w')
for i in range(1000):
	casename = (i + i*i + i*i*i)
	file.write(f"""case {casename}:\nprintf("shit{casename}");\nbreak;\n""")