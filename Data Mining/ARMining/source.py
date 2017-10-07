from collections import Counter
from itertools import combinations

def load_dataset(file = 'small_basket.dat'):
	dataset = []

	with open(file, 'r') as market_basket:
		for line in market_basket.readlines():
			transaction = [int(item.strip()) for item in line.split(',')]
			dataset.append(transaction[1:])

	return dataset


def load_inventory(file = 'products'):
	inventory = []

	with open(file, 'r') as products:
		for line in products.readlines():
			inventory.append(line.partition(', ')[0])

	return inventory


def generate_item_frequencies(dataset, inventory):
	item_frequencies = Counter()

	for transaction in dataset:
		for item_id, item in enumerate(transaction):
			if item != 0:
				item_frequencies[inventory[item_id]] += 1

	return item_frequencies


def generate_C1(dataset, inventory, item_frequencies, min_support):
	C1 = set()
	supports = {}

	for key in item_frequencies:
		item_support = float(item_frequencies[key]) / len(dataset)
		if item_support >= min_support:
			C1.add(frozenset((key,)))
			supports[key] = item_support

	print("Generated C1 size: %s" % str(len(C1)))

	return C1, supports


def self_join_apriori_itemset(apriori_itemset, k):
	Ck = set()

	for anchor_subset in apriori_itemset:
		for joining_subset in apriori_itemset:

			if anchor_subset is joining_subset:
				continue

			else:
				joint_subset = anchor_subset | joining_subset
				if len(joint_subset) != k or joint_subset in Ck:
					continue
				else:
					Ck.add(joint_subset)

	# Although variable is named 'Ck', note that it is pre-pruned!
	return Ck


def generate_subset_family(apriori_itemset, num_choices):
	return combinations(apriori_itemset, num_choices)


def prune_self_joined_itemset(dataset, inventory, Ck, supports_1, k, apriori_itemset):
	lifts_k = compute_lifts(dataset, inventory, Ck, supports_1)
	#print("Lifts for k = %s:\n%s\n" % (str(k), str(lifts_k)))

	for itemset in Ck.copy():
		k_minus_1_subset_family = generate_subset_family(itemset, k - 1)

		for subset in k_minus_1_subset_family:
			if frozenset(subset) not in apriori_itemset:
				#print("Pruning subset: %s\n" % str(subset))
				Ck.remove(itemset)

	for itemset in Ck.copy():
		if lifts_k[itemset] < 1:
			Ck.remove(itemset)

	#print("C%s after pruning:\n%s\n" % (str(k), str(Ck)))
	return Ck


def generate_candidates(apriori_itemset, k, dataset, inventory, supports_1):
	pre_pruned_Ck = self_join_apriori_itemset(apriori_itemset, k)
	#print("Pre-pruned C%s:\n%s\n" % (str(k), str(pre_pruned_Ck)))

	pruned_Ck = prune_self_joined_itemset(dataset, inventory, pre_pruned_Ck, supports_1, k, apriori_itemset)
	#print("Generated C%s:\n%s\n" % (str(k), str(pruned_Ck)))

	return pruned_Ck


def compute_supports(dataset, inventory, candidates):
	supports = {}
	subset_frequencies = Counter()

	for transaction in dataset:
		for itemset in candidates:

			is_subset_of_transaction = True
			for item in itemset:
				if transaction[inventory.index(item)] == 0:
					is_subset_of_transaction = False

			if is_subset_of_transaction == True:
				subset_frequencies[itemset] += 1

	for key in subset_frequencies:
		supports[key] = float(subset_frequencies[key]) / len(dataset)

	return subset_frequencies, supports


def compute_confidences(dataset, inventory, subset_frequencies, item_frequencies):
	confidences = {}

	for subset in subset_frequencies:
		for item in subset:
			rule_key = list(subset)
			rule_key.remove(item)

			confidence = subset_frequencies[subset] / item_frequencies[item]
			confidences["%s => %s" % (str(rule_key), item) ] = confidence

	return confidences


def compute_lifts(dataset, inventory, candidates, supports_1):
	lifts = {}

	candidate_frequencies, candidate_supports = compute_supports(dataset, inventory, candidates)

	for itemset in candidates:
		denominator = 1
		for item in itemset:
			denominator *= supports_1[item]

		lifts[itemset] = candidate_supports[itemset] / denominator

	return lifts


"""
	driver function.
		1. generates k-item candidate itemsets from (k-1)-item subsets
		2. scans database to find candidate itemsets with sufficient support (>= min support)
		3. generates association rules from candidates with sufficient confidence (>= min confidence)
"""

def return_frequent_itemsets_and_rules(dataset, inventory, apriori_itemset, k, supports_1, item_frequencies, \
	min_support, min_confidence): 
	new_frequent_itemsets = set()
	new_association_rules = {}

	Ck = generate_candidates(apriori_itemset, k, dataset, inventory, supports_1)

	subsets_k_frequencies, supports_k = compute_supports(dataset, inventory, Ck)

	confidences_k = compute_confidences(dataset, inventory, subsets_k_frequencies, item_frequencies)

	for key in supports_k:
		if supports_k[key] >= min_support:
			new_frequent_itemsets.add(key)

	print("Frequent itemsets in C%s with sufficient support (%s itemsets):\n" \
		% (str(k), str(len(new_frequent_itemsets))))

	for key in confidences_k:
		if confidences_k[key] >= min_confidence:
			new_association_rules[key] = confidences_k[key]

	print("Association rules generated from C%s with sufficient confidence (%s rules):" \
		% (str(k), str(len(new_association_rules))))

	for key in new_association_rules:
		print("%s: %s" % (key, str(new_association_rules[key])))
	print()

	# package everything needed for next iteration into a single tuple
	return new_frequent_itemsets, new_association_rules