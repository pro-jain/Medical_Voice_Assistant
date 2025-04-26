from datasets import load_dataset

dataset = load_dataset("json", data_files="fine_tune_ready_data.json", split="train")

print(dataset[0])


