import stackless

channel = stackless.channel()

def receiving_tasklet():
    print("receiving taskklet started")
    print(channel.receive())
    print("Receiving tasklet finished")


def sending_tasklet():
    print("Sending tasklet started")
    channel.send("send from sending_tasklet")
    print("sending tasklet finished")

def another_tasklet():
    print("Just another tasklet in the scheduler")
    
def print_x(x):
    print(x)

def print_three_times(x):
    print('1:', x)
    stackless.schedule()
    print('2:', x)
    stackless.schedule()
    print('3:', x)
    stackless.schedule()

ping_channel = stackless.channel()
pong_channel = stackless.channel()

def ping():
    while ping_channel.receive():
        print('PING')
        pong_channel.send("from ping")

def pong():
    while pong_channel.receive():
        print('PONG')
        ping_channel.send('from pong')
        
if __name__ == '__main__':
    stackless.tasklet(print_x)('one')
    stackless.tasklet(print_x)('two')
    stackless.tasklet(print_x)('three')
    stackless.tasklet(print_three_times)('first')
    stackless.tasklet(print_three_times)('second')
    stackless.tasklet(print_three_times)('third')
    stackless.tasklet(receiving_tasklet)()
    # stackless.tasklet(sending_tasklet)()
    stackless.tasklet(another_tasklet)()

    stackless.tasklet(ping)()
    stackless.tasklet(pong)()
    stackless.tasklet(ping_channel.send)('startup')
    stackless.run()
