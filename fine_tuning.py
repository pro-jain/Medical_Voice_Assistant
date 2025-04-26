from datasets import Dataset
from transformers import AutoTokenizer, AutoModelForCausalLM, TrainingArguments, Trainer
import json


with open("pills.json", "r") as f:
    data = json.load(f)


dataset = Dataset.from_list(data)


model_name = "gpt2"  
tokenizer = AutoTokenizer.from_pretrained(model_name)


tokenizer.pad_token = tokenizer.eos_token


def tokenize(example):
    prompt = f"### Question:\n{example['prompt']}\n\n### Answer:\n{example['response']}"
    return tokenizer(prompt, truncation=True, padding="max_length", max_length=512)


tokenized_dataset = dataset.map(tokenize, batched=True)
 

args = TrainingArguments(
    output_dir="output",
    per_device_train_batch_size=1,
    num_train_epochs=3,
    logging_steps=10,
    save_steps=50,
    save_total_limit=2
)


model = AutoModelForCausalLM.from_pretrained(model_name)

trainer = Trainer(
    model=model,
    args=args,
    train_dataset=tokenized_dataset
)

trainer.train()
