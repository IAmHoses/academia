# like really hard

import Records
import Cluster

import copy
import math
import random

class Buckshot:
	"""
		docstring for Buckshot
	"""

	def __init__(self):
		clean_data = Records.load_preprocessed_data()
		self.clean_records = clean_data[0]
		self.class_1_records = clean_data[1]
		self.class_2_records = clean_data[2]

		self.cluster_seed = []
		self.remaining_records = None

		self.k_clusters = None


	def seed(self):
		records_copy = copy.deepcopy(self.clean_records)

		for i in range(math.ceil(math.sqrt(len(records_copy)))):
			random_idx = int(random.uniform(0, len(records_copy) - 1))

			self.cluster_seed.append(Cluster.Cluster(records_copy[random_idx]))
			records_copy.remove(records_copy[random_idx])

		self.remaining_records = records_copy

		print("Seed of %s clusters obtained for buckshot classification.\n" % str(len(self.cluster_seed)))


	def HAC(self, k):
		print("Performing Hierarchical Agglomerative Clustering...")

		while len(self.cluster_seed) >= k:
			minDistance = None
			minCell = None

			for i, cluster1 in enumerate(self.cluster_seed):
				for j, cluster2 in enumerate(self.cluster_seed):
					if cluster1 == cluster2:
						continue

					cluster_dist = cluster1.centroid_distance(cluster2)

					if minDistance == None and minCell == None:
						minDistance = cluster_dist
						minCell = (i, j)

					elif cluster_dist < minDistance:
						minDistance = cluster_dist
						minCell = (i, j)

			# cleanup before termination
			if len(self.cluster_seed) == k:
				self.k_clusters = self.cluster_seed
				print("--> %s clusters generated!\n" % str(k))

				self.cluster_seed[minCell[0]].merge(self.cluster_seed[minCell[1]])
				self.cluster_seed[minCell[0]].update_centroid()

				self.cluster_seed.remove(self.cluster_seed[minCell[1]])
				continue

			#print("\tMinimum cluster distance %s found at cell: %s" % (str(minDistance), str(minCell)))

			self.cluster_seed[minCell[0]].merge(self.cluster_seed[minCell[1]])
			self.cluster_seed[minCell[0]].update_centroid()

			self.cluster_seed.remove(self.cluster_seed[minCell[1]])

			#print("\tClusters remaining in seed: %s\n" % str(len(self.cluster_seed)))

	def sum_squared_error(self):
		SSE = 0

		for cluster in self.k_clusters:
			SSE += cluster.sum_squared_error()

		return SSE

	def min_cluster_distance(self):
		minDistance = None

		for cluster1 in self.k_clusters:
			for cluster2 in self.k_clusters:
				if cluster1 == cluster2:
					continue

				cluster_dist = cluster1.centroid_distance(cluster2)

				if minDistance == None:
					minDistance = cluster_dist

				elif cluster_dist < minDistance:
					minDistance = cluster_dist

		return minDistance

	def max_cluster_distance(self):
		maxDistance = None

		for cluster1 in self.k_clusters:
			for cluster2 in self.k_clusters:
				if cluster1 == cluster2:
					continue

				cluster_dist = cluster1.centroid_distance(cluster2)

				if maxDistance == None:
					maxDistance = cluster_dist

				elif cluster_dist > maxDistance:
					maxDistance = cluster_dist

		return maxDistance

	def min_cluster_size(self):
		minSize = None

		for cluster in self.k_clusters:
			if minSize == None:
				minSize = cluster.size()

			elif cluster.size() < minSize:
				minSize = cluster.size()

		return minSize

	def max_cluster_size(self):
		maxSize = None

		for cluster in self.k_clusters:
			if maxSize == None:
				maxSize = cluster.size()

			elif cluster.size() > maxSize:
				maxSize = cluster.size()

		return maxSize

	def avg_cluster_size(self):
		size_total = 0

		for cluster in self.k_clusters:
			size_total += cluster.size()

		return size_total / len(self.k_clusters)

	def single_pass_assignment(self):
		print("Assigning remaining records to generated clusters...")

		for record in self.remaining_records:
			minDistance = None
			minIndex = None

			for i, cluster in enumerate(self.k_clusters):
				distance = cluster.euclidian_distance(cluster.get_centroid(), record)

				if minDistance == None and minIndex == None:
					minDistance = distance
					minIndex = i

				elif distance < minDistance:
					minDistance = distance
					minIndex = i

			self.k_clusters[i].add_entry(record)

		print("Single-pass record assignment complete!\n")

	def accuracy(self):
		tp_fn = 0

		for cluster in self.k_clusters:
			for record in cluster.get_entries():
				if record.class_label == cluster.get_centroid().class_label:
					tp_fn += 1

		return tp_fn / len(self.clean_records)

	def macro_avg_precision(self):
		cluster_precisions = 0

		for cluster in self.k_clusters:
			cluster_precisions += cluster.precision()

		return cluster_precisions / len(self.k_clusters)

	def micro_avg_precision(self):
		correct_class = 0
		cluster_sizes = 0

		for cluster in self.k_clusters:
			cluster_sizes += cluster.size()

			for record in cluster.get_entries():
				if record.class_label == cluster.get_centroid().class_label:
					correct_class += 1

		return correct_class / cluster_sizes

def main():
	b = Buckshot()
	b.seed()
	b.HAC(15)
	b.single_pass_assignment()

	print("Sum Squared Error: %s" % str(b.sum_squared_error()))
	print("Min Cluster Distance: %s" % str(b.min_cluster_distance()))
	print("Max Cluster Distance: %s\n" % str(b.max_cluster_distance()))

	print("Min Cluster Size: %s" % str(b.min_cluster_size()))
	print("Max Cluster Size: %s" % str(b.max_cluster_size()))
	print("Average Cluster Size: %s\n" % str(b.avg_cluster_size()))

	print("Accuracy: %s" % str(b.accuracy()))
	print("Macro Average Precision: %s" % str(b.macro_avg_precision()))
	print("Micro Average Precision: %s" % str(b.micro_avg_precision()))

if __name__ == '__main__':
	main()