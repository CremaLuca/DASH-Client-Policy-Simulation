import matplotlib.pyplot as plt
import subprocess
import re


def ping(addr: str, length: int, tries: int = 2):
    response = None
    while(response == None):
        try:
            response = subprocess.check_output(
                ['ping', '-l', str(length), '-n', str(tries), addr])
        except:
            print("Non sono riuscito a fare il ping con -l = ", length)
    values = re.findall(pattern=r'[\d]+ms', string=response.decode("UTF-8"))
    minimum = int(values[len(values)-3][:-2])
    maximum = int(values[len(values)-2][:-2])
    average = int(values[len(values)-1][:-2])
    return (minimum, maximum, average)


if __name__ == "__main__":
    mins = list()
    maxs = list()
    avgs = list()
    lengths = list()
    rates = list()
    for i in range(0, 15000, 100):
        values = ping("88.80.187.84", i)
        mins.append(values[0])
        maxs.append(values[1])
        avgs.append(values[2])
        rates.append(26 * (((i + 28)*8)/(values[0])))
        lengths.append(i)
    #print("mins: {}, maxs: {}, avgs: {}".format(mins,maxs,avgs))
    Rstar = 26 * ()
    mins_plot = plt.plot(lengths, mins, label="Mins")
    maxs_plot = plt.plot(lengths, maxs, label="Maxs")
    avgs_plot = plt.plot(lengths, avgs, label="Avgs")
    rate_plot = plt.plot(lengths, rates, label="Rates")
    plt.legend([mins_plot, maxs_plot, avgs_plot, rate_plot], ['Minimums', 'Maximums','Averages','R*'])
    plt.show()
