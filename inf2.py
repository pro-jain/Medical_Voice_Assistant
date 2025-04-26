import os
import platform
from transformers import AutoTokenizer, AutoModelForCausalLM
from gtts import gTTS
import speech_recognition as sr


os.environ["TOKENIZERS_PARALLELISM"] = "false"

model_path = "./results"
tokenizer = AutoTokenizer.from_pretrained(model_path)
model = AutoModelForCausalLM.from_pretrained(model_path)


def speak_with_gtts(text):
    if not text.strip():
        print("No text to speak.")
        return
    tts = gTTS(text=text, lang='en')
    tts.save("response.mp3")

    if platform.system() == "Windows":
        os.system("start response.mp3")
    elif platform.system() == "Darwin":  
        os.system("afplay response.mp3")
    else:  # Linux
        os.system("mpg123 response.mp3")


def get_voice_input():
    recognizer = sr.Recognizer()
    with sr.Microphone() as source:
        print("\n Listening... Speak your question (or say 'exit' to quit):")
        recognizer.adjust_for_ambient_noise(source, duration=1)  
        audio = recognizer.listen(source)

    try:
        text = recognizer.recognize_google(audio)
        print(f"You said: {text}")
        return text
    except sr.UnknownValueError:
        print(" Could not understand audio. Please try again.")
        return None
    except sr.RequestError as e:
        print(f"Could not request results; {e}")
        return None


def clean_and_format_input(text):
    text = text.strip()
    if not text:
        return text
    
    text = text[0].upper() + text[1:]
    
    if not text.endswith('?') and any(word in text.lower() for word in ['what', 'why', 'how', 'when', 'where', 'who']):
        text += '?'
    return text


def limit_to_60_words(text):
    words = text.split()
    if len(words) > 60:
        words = words[:60]
    return ' '.join(words)


while True:
    user_input = get_voice_input()
    if user_input is None:
        continue
    if user_input.lower() == 'exit':
        break

    formatted_input = clean_and_format_input(user_input)
    print(f"[CLEANED INPUT] -> {formatted_input}")

    prompt = f"""### Instruction:
{formatted_input}

### Response:"""

    inputs = tokenizer(prompt, return_tensors="pt")
    outputs = model.generate(
        **inputs,
        max_new_tokens=150,
        pad_token_id=tokenizer.eos_token_id,
        temperature=0.7,
        top_p=0.9,
        repetition_penalty=1.2,
        do_sample=True
    )

    response = tokenizer.decode(outputs[0], skip_special_tokens=True)
    print("\n[DEBUG] Raw model output:\n", response)

    if "### Response:" in response:
        final_response = response.split("### Response:")[-1].strip()
    else:
        final_response = response.strip()


    final_response = limit_to_60_words(final_response)

    if final_response:
        print("\n " + final_response)
        speak_with_gtts(final_response)
    else:
        print("\n (No valid response generated. Trying again...)")
        continue

