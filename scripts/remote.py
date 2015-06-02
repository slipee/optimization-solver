import paramiko
import time
import os

ssh = None
sftp = None

def connect(options):
    key = paramiko.RSAKey.from_private_key_file(options['private_key_path'])
    global ssh
    ssh = paramiko.SSHClient()
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())

    print "Connecting..."
    ssh.connect(hostname=options['host'], username=options['user'], pkey=key, password=options['password'])

    print "Sending..."
    global sftp
    sftp = ssh.open_sftp()
    sftp.put('src.tar', 'src.tar')

    stdin , stdout, stderr = ssh.exec_command('tar -xzf src.tar')
    print stdout.read(), stderr.read()


def compileProgram():
    print "Compiling..."
    stdin , stdout, stderr = ssh.exec_command('. .profile ; make CPL="mpixlcxx"')
    print stdout.read(), stderr.read()


def startProgram(processNumber, fileName, errFile, options):
    optionsStr = " ".join(options)

    print "Executing..."
    stdin , stdout, stderr = ssh.exec_command('mpisubmit.bg -n %s -w 00:10:00 --stdout %s  --stderr %s optimization.out -- %s' % (processNumber, fileName, errFile, optionsStr))
    print stdout.read(), stderr.read()

def closeConnection():
    sftp.close()
    ssh.close()

def getResultFile(fileName, errFile):
    get = 0
    while not get:
        try:
            get = 1

            try:
                sftp.get(errFile, errFile)
                sizeFile = os.stat(errFile).st_size
                if sizeFile != 0:
                    print "Something went wrong"
                    return
            except:
                pass

            sftp.get(fileName, fileName)
            sizeFile = os.stat(fileName).st_size
            if sizeFile == 0:
                os.remove(fileName)
                get = 0
        except Exception, e:
            get = 0
            time.sleep(30)







