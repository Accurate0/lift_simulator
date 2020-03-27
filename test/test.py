#!/usr/bin/env python3
from threading import Thread
from queue import Queue
from os import listdir
from os.path import isfile, join
import shlex
import subprocess
from subprocess import DEVNULL

EXEC = './lift_sim_A {} {} {}'
VALGRIND = 'valgrind --track-origins=yes \
                     --leak-check=full \
                     --show-leak-kinds=all \
                     --show-reachable=yes \
                     --error-exitcode=2 {}'
HELGRIND = 'valgrind --tool=helgrind --error-exitcode=2 {}'
TEST_DIR = 'test/inputs/'

QUEUE_SIZE = 5
LIFT_TIME  = 0

class Result(object):
    def __init__(self, name, file, ret):
        self.name = name
        self.file = file
        self.ret = ret

class TaskThread(Thread):
    def __init__(self, q: Queue, tasks: list):
        super().__init__()
        self._q = q
        self._tasks = tasks

    def run(self):
        for task in self._tasks:
            cmd = shlex.split(task)
            ret = subprocess.run(cmd, stdout=DEVNULL, stderr=DEVNULL)
            self._q.put(Result(task, cmd[-1], ret.returncode))

        self._q.put('end')

# Stolen straight from stackoverflow
def chunks(lst, n):
    """Yield successive n-sized chunks from lst."""
    for i in range(0, len(lst), n):
        yield lst[i:i + n]

def recompile():
    subprocess.run(shlex.split('make clean'))
    subprocess.run(shlex.split('make VALGRIND=1'))

def main():
    recompile()
    files = [join(TEST_DIR, f) for f in listdir(TEST_DIR) if isfile(join(TEST_DIR, f))]
    maxlen = len(max(files, key=len))

    total_tasks = [VALGRIND.format(EXEC.format(QUEUE_SIZE, LIFT_TIME, file)) for file in files] \
                + [HELGRIND.format(EXEC.format(QUEUE_SIZE, LIFT_TIME, file)) for file in files] \
                + [EXEC.format(QUEUE_SIZE, LIFT_TIME, file) for file in files]

    q = Queue()
    threads = [TaskThread(q, chunk) for chunk in chunks(total_tasks, 10)]

    for thread in threads:
        thread.daemon = True
        thread.start()

    n = 0
    while ret := q.get():
        if type(ret) is str:
            n += 1
            if n == len(threads):
                break
        else:
            name = ''
            if 'helgrind' in ret.name:
                name = 'helgrind'
            elif 'valgrind' in ret.name:
                name = 'valgrind'
            else:
                name = 'regular '

            file = ret.file + (' ' * (maxlen - len(ret.file)))

            print(f'{name} {file} ', end='')

            if ret.ret == 0:
                print('\033[92m[pass]\033[0m')
            else:
                print('\033[91m[fail]\033[0m')

            # print(n)
        q.task_done()

    for thread in threads:
        thread.join()

if __name__ == '__main__':
    main()
