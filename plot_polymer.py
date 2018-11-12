import numpy as np 
import matplotlib.pyplot as plt 


def plot_polymer(polymer):
	'''To plot the polymer or the signal'''
	colors=['gray','darkgreen','darkseagreen','goldenrod','crimson','mediumorchid','purple']
	f,axes = plt.subplots(len(colors), sharex=True,sharey=True)

	axis = [i for i in range(len(polymer))]
	for i,c in zip(range(len(colors)),colors):
		axes[i].bar(axis,polymer[:,i],color=c)
		axes[i].axis([0,len(polymer),0,15])
		axes[i].xaxis.set_visible(False)
		axes[i].yaxis.set_visible(False)
	plt.show()


pol=np.loadtxt("polymers/reference_polymer.txt")
#pol=np.loadtxt("results/example_reordered.txt")
plot_polymer(pol)