import source

dataset = source.load_dataset()
inventory = source.load_inventory()
item_frequencies = source.generate_item_frequencies(dataset, inventory)

C1, supports_1 = source.generate_C1(dataset, inventory, item_frequencies, 0.42)
k = 2

last_frequent_itemset = C1

while last_frequent_itemset != set():
	Ck_frequent_itemset, Ck_supports = source.return_frequent_itemsets_and_rules(dataset, inventory, last_frequent_itemset, \
		k, supports_1, item_frequencies, 0.42, 0.5)
	k+=1
	last_frequent_itemset = Ck_frequent_itemset