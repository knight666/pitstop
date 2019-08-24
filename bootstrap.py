import sys
if sys.version_info < (3, 0):
	sys.stdout.write("Bootstrap requires Python 3.x.\n")
	sys.exit(1)

import argparse
import builtins as __builtin__
import enum
import fileinput
import hashlib
import json
import glob
import os.path
import os
import re
import shutil
import subprocess
import tarfile
import urllib.request
import winreg
import zipfile

PrintLevel = 0
WorkingDir = None
ProgramOptions = None
VsInstallPath = None
DevEnvPath = None
VcVarsPath = None
MsBuildPath = None
QtSdkPath = None
MercurialPath = None
GitPath = None
BuildDir = None
PackageDir = None
PackageTempDir = None
MegaToolsPath = None
MegaLoggedIn = False
TwinePath = None
GameVersion = None

def print(*args, **kwargs):
	global PrintLevel
	s = '\t' * PrintLevel
	return __builtin__.print(s + args[0], **kwargs)

class PrintScopeLock():
	def __init__(self):
		pass

	def __enter__(self):
		global PrintLevel
		PrintLevel = PrintLevel + 1

	def __exit__(self, *args):
		global PrintLevel
		PrintLevel = PrintLevel - 1

def PushWorkingDir(path):
	os.chdir(os.path.abspath(path))

def PopWorkingDir():
	os.chdir(WorkingDir)

def RunCommand(args, expected = 0, vcVars = False, verbose = False):
	if vcVars:
		args = [ VcVarsPath, 'amd64', '&&' ] + args

	if verbose:
		print(str(args))

	with subprocess.Popen(args, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT) as proc:
		for line in iter(proc.stdout.readline, b''):
			print(str(line.rstrip()))
		proc.communicate()
		print('')
		return proc.returncode == expected

def ResolvePath(path):
	path = os.path.abspath(path)
	if not os.path.exists(path):
		os.makedirs(path, 0o777, True)

	return path

def CopyFile(src, dst = None, force = False):
	filename = os.path.basename(src)

	if dst:
		dst = os.path.abspath(dst)
	else:
		dst = os.path.abspath(BuildDir + '/' + filename)

	ResolvePath(os.path.dirname(dst))

	if not os.path.exists(dst) or force:
		print('Copying "' + filename + "'...")
		shutil.copy(src, dst)
		print('')

class GitResult(enum.Enum):
	ok = 1
	warning = 2,
	failed = 3,
	rebuild = 4

def GitCommand(args, silent = False):
	result = GitResult.ok
	args = [ GitPath ] + args
	host_check = re.compile(r".*Couldn't resolve host")

	with PrintScopeLock() as lock:
		with subprocess.Popen(args, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT) as proc:
			for line in iter(proc.stdout.readline, b''):
				stripped = str(line.rstrip())
				if host_check.match(stripped):
					result = GitResult.warning

				if not silent:
					print(stripped)

			proc.communicate()

			if not silent:
				print('')

		if result == GitResult.ok:
			if proc.returncode == 0:
				result = GitResult.ok
			else:
				result = GitResult.failed

		return result

def GitUpdate(dst, src, force=False):
	clone = force

	if not os.path.exists(os.path.abspath(dst + '/README.md')):
		os.mkdir(dst)
		clone = True

	PushWorkingDir(dst)

	if clone or force:
		result = GitCommand(['clone', src, '.'])
	else:
		local = ""
		with subprocess.Popen([GitPath, 'rev-parse', '@'], shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT) as proc:
			for line in iter(proc.stdout.readline, b''):
				local = str(line.rstrip())
				break
			proc.communicate()

		remote = ""
		with subprocess.Popen([GitPath, 'rev-parse', 'master'], shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT) as proc:
			for line in iter(proc.stdout.readline, b''):
				remote = str(line.rstrip())
				break
			proc.communicate()

		if local != remote:
			result = GitCommand(['pull', 'origin', 'master'])
			if result == GitResult.ok:
				result = GitResult.rebuild
		else:
			result = GitResult.ok

	PopWorkingDir()

	with PrintScopeLock() as lock:
		if result == GitResult.failed:
			print('Failed to clone or update "' + src + '".')
			print('')

	return result

def CmakeCommand(args, silent = False):
	with PrintScopeLock() as lock:
		args = [ 'cmake' ] + args
		error_check = re.compile('.*Error: (.*)')
		build_check = re.compile('Build FAILED')

		print(str(args))

		with subprocess.Popen(args, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT) as proc:
			for line in iter(proc.stdout.readline, b''):
				stripped = str(line.rstrip())

				error_match = error_check.match(stripped)
				if error_match:
					print(error_match.group(1))
					print('')

					return False

				if build_check.match(stripped):
					print('')

					return False

				if not silent:
					print(stripped)

			proc.communicate()

			if not silent:
				print('')

		return True

# tools

def ToolsVisualStudio():
	global VsInstallPath
	global DevEnvPath
	global VcVarsPath
	global MsBuildPath

	print('Visual Studio 2017:')

	with PrintScopeLock() as lock:
		args = [ r'%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe' ]
		args.extend([ '-latest' ])
		args.extend([ '-legacy' ])
		args.extend([ '-property', 'installationPath' ])

		with subprocess.Popen(args, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT) as proc:
			for line in iter(proc.stdout.readline, b''):
				VsInstallPath = os.path.abspath(str(line.rstrip(), encoding='ascii'))
				break

		if not VsInstallPath:
			print('Cannot find Visual Studio executable.')
			print('')

			return False

		DevEnvPath = os.path.abspath(VsInstallPath + r'\Common7\IDE\devenv.com')
		if not os.path.exists(DevEnvPath):
			print('Failed to find path to devenv.com')
			print('')

			return False

		VcVarsPath = os.path.abspath(VsInstallPath + r'\VC\Auxiliary\Build\vcvarsall.bat')
		if not os.path.exists(VcVarsPath):
			print('Failed to find path to vcvars.bat')
			print('')

			return False

		print('INSTALLED')
		print('')

	print('MSBuild:')

	with PrintScopeLock() as lock:
		MsBuildPath = os.path.abspath(VsInstallPath + r'\MSBuild\15.0\Bin\MSBuild.exe')
		if not os.path.exists(MsBuildPath):
			print('Cannot find MSBuild executable for Visual Studio 2017.')
			print('')

			return False

		print('INSTALLED')
		print('')

	return True

def ToolsGit():
	global GitPath

	print('Git:')

	with PrintScopeLock() as lock:
		version = None

		paths = [
			'git',
			os.path.abspath(os.getenv('LOCALAPPDATA') + r'\Atlassian\SourceTree\git_local\bin\git.exe'),
			os.path.abspath(VsInstallPath + r'\IDE\Extensions\Microsoft\Web Tools\External\git\git.exe')
		]

		for path in paths:
			try:
				p = subprocess.Popen([path, '--version'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
				for line in iter(p.stdout.readline, b''):
					match = re.match('.*version ([0-9]+\\.[0-9]+\\.[0-9]+)', str(line))
					if match:
						version = match.group(1)
						break

				if version:
					GitPath = path
					break
			except FileNotFoundError:
				continue

		if not version:
			print('Failed to find git installation.')
			print('')

			return False

		#print(GitPath)

		print('INSTALLED ' + version)
		print('')

		return True

def ToolsCMake():
	print('CMake:')

	with PrintScopeLock() as lock:
		try:
			key = winreg.OpenKey(winreg.HKEY_CURRENT_USER, r"SOFTWARE\Kitware\CMake", 0, winreg.KEY_READ)
			installed = winreg.QueryValueEx(key, 'installed')
		except OSError:
			print('CMake is not installed.')
			print('')

			return False
		
		if installed[0] != 1:
			print('CMake is not installed.')
			print('')

			return False

		version = None

		p = subprocess.Popen(['cmake', '--version'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		for line in iter(p.stdout.readline, b''):
			match = re.match('.*version ([0-9]+\\.[0-9]+\\.[0-9]+)', str(line))
			if match:
				version = match.group(1)
				break

		if not version:
			print('Failed to determine CMake version.')
			print('')

			return False

		print('INSTALLED ' + version)
		print('')

		return True

def ToolsQt():
	global QtSdkPath

	print('Qt:')

	with PrintScopeLock() as lock:
		if os.getenv('QT5SDKDIR') == None:
			print('Environment variable "QT5SDKDIR" must be set manually to "<PathToQtSDK>\\Qt<Version>\\msvc2017_64\\".')
			print('')

			return False

		QtSdkPath = os.getenv('QT5SDKDIR')

		# use qmake to determine version

		version = None

		p = subprocess.Popen([ os.path.abspath(QtSdkPath + r'\bin\qmake'), '--version' ], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		for line in iter(p.stdout.readline, b''):
			match = re.match(r'.*Qt version ([0-9]+\.[0-9]+\.[0-9]+)', str(line))
			if match:
				version = match.group(1)
				break

		if not version:
			print('Failed to determine Qt version.')
			print('')

			return False

		# check installation

		match = re.match(r'([0-9]+)\.([0-9]+)\.([0-9]+)', version)
		if not match:
			print('Qt version number has incorrect format "%s".' % (version))
			print('')

			return False

		if int(match.group(1)) < 5 or int(match.group(2)) < 9:
			print('Qt %s is out of date, Qt >= 5.9.x is required.' % (version))
			print('')

			return False

		print('INSTALLED ' + version)
		print('')

	return True

# dependencies

def DependencyQt():
	print('Qt Modules:')

	with PrintScopeLock() as lock:
		bin_dir = QtSdkPath + '/bin'

		qtlibs = [
			'Qt5Core',
			'Qt5Gui',
			'Qt5Widgets',
		]
		for lib in qtlibs:
			CopyFile(bin_dir + '/' + lib + '.dll')
			CopyFile(bin_dir + '/' + lib + 'd.dll')

		print('INSTALLED')
		print('')

		return True

def BuildApplication():
	dst_path = os.path.abspath('.')

	print('Building application...')
	print('')

	with PrintScopeLock() as lock:
		args = [DevEnvPath, dst_path + '\\Pitstop.sln', '/Project', 'Pitstop', '/Build', 'Release']
		if not RunCommand(args, vcVars=True):
			print('Failed to build application.')
			print('')

			return False

		print('SUCCEEDED')
		print('')

		return True

def DeployQt():
	print('Running Qt deploy tool...')
	print('')

	with PrintScopeLock() as lock:
		debug_exe = os.path.abspath(BuildDir + r'\PitstopDebug.exe')
		if os.path.exists(debug_exe):
			print('Deploying debug...')
			print('')
			RunCommand([QtSdkPath + r'\bin\windeployqt.exe', debug_exe], vcVars=True)

		release_exe = os.path.abspath(BuildDir + r'\PitstopRelease.exe')
		if os.path.exists(release_exe):
			print('Deploying release...')
			print('')
			RunCommand([QtSdkPath + r'\bin\windeployqt.exe', release_exe], vcVars=True)

		print('SUCCEEDED')
		print('')

		return True

def RunBuild():
	exe_path = os.path.abspath(BuildDir + r'\PitstopRelease.exe')

	print('Running "%s"...' % (os.path.split(exe_path)[1]))
	print('')

	with PrintScopeLock() as lock:
		if not os.path.exists(exe_path):
			print('Missing executable.')
			print('')

			return False

		with subprocess.Popen(exe_path) as game:
			game.communicate()

		print('SUCCEEDED')
		print('')

	return True

if __name__ == '__main__':
	WorkingDir = os.getcwd()

	parser = argparse.ArgumentParser(description='Bootstraps project and dependencies')
	parser.add_argument(
		'--force',
		dest = 'force',
		action = 'store_true',
		help = 'do not skip steps if they were already completed'
	)
	parser.add_argument(
		'--build',
		dest = 'build',
		action = 'store_true',
		help = 'build application'
	)
	parser.add_argument(
		'--deploy',
		dest = 'deploy',
		action = 'store_true',
		help = 'deploy qt dlls'
	)
	parser.add_argument(
		'--run',
		dest = 'run',
		action = 'store_true',
		help = 'run the application'
	)
	ProgramOptions = parser.parse_args()

	# default is build and run

	if len(sys.argv) == 1:
		ProgramOptions.build = True
		ProgramOptions.run = True

	# globals

	BuildDir = ResolvePath('Build')

	# dependencies

	dependencies = []

	def AddDependency(dep):
		if dep not in dependencies:
			dependencies.append(dep)

	if ProgramOptions.build or ProgramOptions.deploy:
		AddDependency(ToolsVisualStudio)
		AddDependency(ToolsQt)
		AddDependency(ToolsCMake)
		AddDependency(ToolsGit)
		AddDependency(DependencyQt)

	if len(dependencies) > 0:
		print('[  DEPENDENCIES  ]')
		print('')

		for dep in dependencies:
			if not dep():
				exit(1)

	# app

	print('[  APPLICATION   ]')
	print('')

	if ProgramOptions.build:
		if not BuildApplication():
			exit(1)

	if ProgramOptions.deploy or ProgramOptions.build:
		DeployQt()

	if ProgramOptions.run:
		if not RunBuild():
			exit(1)

	print('[      DONE      ]')

	exit(0)