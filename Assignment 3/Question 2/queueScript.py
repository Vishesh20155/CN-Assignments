import matplotlib.pyplot as plt

file = open("tcp-example.tr","r")
lines = file.readlines()

enque_time = {}
points = {}


for l1 in lines:
    
    l1_components = l1.split()
    node_det = l1_components[2]
    seq_no = l1_components[36]
    t = float(l1_components[1])
    if node_det[10] == '1' and node_det[23] == '1':
        if l1_components[0] == '+':
            enque_time[seq_no] = t
        elif l1_components[0] == '-':
            points[t] = t - enque_time[seq_no]


plt.xlabel("Time (seconds)")
plt.ylabel("Queueing Delay (seconds)")
plt.plot(points.keys(), points.values())
plt.show()


file.close()
