from transformers import AutoTokenizer, AutoModelForCausalLM, TrainingArguments, Trainer, DataCollatorForLanguageModeling
from datasets import load_dataset, Dataset
import json
import torch


with open("pills.json", "r") as f:
    data = json.load(f)

formatted_data = [
    {"text": f"### Instruction:\n{item['instruction']}\n\n### Response:\n{item['output']}"}
    for item in data
]

dataset = Dataset.from_list(formatted_data)


model_name = "gpt2"
tokenizer = AutoTokenizer.from_pretrained(model_name)
tokenizer.pad_token = tokenizer.eos_token

model = AutoModelForCausalLM.from_pretrained(model_name)


def tokenize(example):
    return tokenizer(example["text"], truncation=True, padding="max_length", max_length=512)

tokenized_dataset = dataset.map(tokenize)


args = TrainingArguments(
    output_dir="./results",
    per_device_train_batch_size=2,
    num_train_epochs=5,
    save_steps=500,
    logging_steps=100,
    learning_rate=5e-5,
    save_total_limit=1,
    fp16=True if torch.cuda.is_available() else False
)

data_collator = DataCollatorForLanguageModeling(tokenizer=tokenizer, mlm=False)


trainer = Trainer(
    model=model,
    args=args,
    train_dataset=tokenized_dataset,
    data_collator=data_collator
)


trainer.train()


model.save_pretrained("./results")
tokenizer.save_pretrained("./results")
