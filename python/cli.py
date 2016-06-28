import sys
from enum import Enum
import utl.log as log

class Queue(list):
	items = []
	def peek(self):
		return self.items[0] if len(self.items) > 0 else None
	def get(self):
		i = self.items[0]
		self.items = self.items[1:] if len(self.items) > 1 else []
		return i
	def put(self, item):
		self.items.append(item)
	def empty(self):
		return len(self.items) == 0

class Data(Enum):
	NoData = 0
	Optional = 1
	Required = 2


class Flag(object):
	shortName = '' # Short name of flag (i.e. -i, -u john.doe).
	longName = '' # Long name of flag (i.e. --interactive, --username=john.doe).
	required = False # True if the argument is required.
	description = '' # Description of what the argument does.
	data = Data.NoData # Takes the next argument in the argument list as data.
	delegate = None # Method to be called, must take one parameter. Will be the paired data, or None if data is False.
	def __init__(self, delegate, shortName:str = None, longName:str = None, required:bool = False, description:str = None, data:Data = Data.NoData):
		'''Initializes the Flag class'''
		self.shortName = shortName
		self.longName = longName
		self.required = required
		self.description = description
		self.data = data
		self.delegate = delegate
	def __str__(self):
		return self.shortName if self.shortName != None and len(self.shortName) > 0 else self.longName

class CLI(object):
	flags = None
	description = None
	startDelegate = None
	argv = None
	stderr = None
	
	def __init__(self, start, flags:list, argv:list = sys.argv, description:str = None, stderr = sys.stderr):
		self.startDelegate = start
		self.flags = flags
		self.description = description
		self.argv = argv
		self.stderr = stderr
		
	def run(self):
		args = Queue()
		flags = Queue()
		reqs = [x for x in self.flags if x.required == True]
		unknown = []
		if(len(self.argv) > 1 and self.argv[1] == '--help'):
			self.showHelp()
		for arg in self.argv[1:]:
			args.put(arg)
		while not args.empty():
			rawArg = args.get()
			arg = self._getArg(rawArg)
			flag = self._getFlag(arg[1], arg[0])
			if(flag == None):
				unknown.append(rawArg)
				continue
			if(flag in reqs):
				reqs.remove(flag)
			data = None
			if(not flag.data == Data.NoData):
				if(arg[1] == flag.shortName):
					tmp = self._getArg(args.peek())
					isFlag = self._getFlag(tmp[1], tmp[0]) != None
					if(flag.data == Data.Required and (args.empty() or isFlag)):
						self.showHelp("Expected data for flag '{0}'".format(arg[1]))
						return
					elif(flag.data == Data.Required or (flag.data == Data.Optional and not args.empty() and not isFlag)):
						data = args.get()
				else:
					if('=' in rawArg):
						data = '='.join(rawArg.split('=')[1:])
					else:
						self.showHelp("Expected data for flag '{0}'".format(arg[1]))
						return
			flag.delegate(data)
		if(len(reqs) > 0):
			self.showHelp('Missing required arguments [{0}]'.format(', '.join([str(x) for x in reqs])))
			return
		self.startDelegate(unknown)

	def showHelp(self, error:str = None):
		if(error != None):
			print(error, file=self.stderr)
		print('usage: {0} [options]'.format(sys.argv[0]) if self.description == None else self.description, file=self.stderr)
		shortLen = 0
		longLen = 0
		for flag in self.flags:
			shortLen = len(flag.shortName) if flag.shortName != None and len(flag.shortName) > shortLen else shortLen
			longLen = len(flag.longName) if flag.longName != None and len(flag.longName) > longLen else longLen
		for flag in self.flags:
			shortName = flag.shortName if flag.shortName != None and len(flag.shortName.strip()) else ''
			longName = flag.longName if flag.longName != None and len(flag.longName.strip()) > 0 else '    '
			description = flag.description if flag.description != None else ''
			cmdStr = ('  {0:<'+str(shortLen)+'} {1:<'+str(longLen)+'} ').format(shortName, longName)
			
			newlineIndecies = []
			count = 0
			max = 80 - len(cmdStr)
			for c in range(len(description)):
				if description[c] == '\n':
					count = 0
				else:
					count += 1
				if count == max:
					newlineIndecies.append(c)
					count = 0
			count = 0
			for index in newlineIndecies:
				description = description[:index+count] + '\n' + description[index+count:]
				count += 1
			
			description = description.replace('\n','\n'.ljust(len(cmdStr) + 1))# add one so as to ignore the newline during the ljust
			print(cmdStr + description, file=self.stderr)
	def _getArg(self, arg:str):
		if(arg == None):
			return (False, None)
		return (True, arg.split('=')[0]) if '=' in arg else (False, arg)
	def _getFlag(self, name:str, isLong:bool):
		if(name == None):
			return None
		return next((x for x in self.flags if x.longName == name), None) if isLong == True else next((x for x in self.flags if x.shortName == name), None)

# def flag1(data):
	# print('flag1: ' + (data if data != None else ''))
# def flag2(data):
	# print('flag2: ' + (data if data != None else ''))
# def start(args):
	# for arg in args:
		# print(arg)

# cli = CLI(start, [
# Flag(flag1, '-f', '--flag1', True, 'Example flag 1', Data.Required),
# Flag(flag2, '-g', '--flag2', False, 'Example flag 2', Data.Optional)
# ], [])
# cli.run()
