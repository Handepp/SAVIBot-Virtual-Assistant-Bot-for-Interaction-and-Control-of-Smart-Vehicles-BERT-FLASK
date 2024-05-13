import re
import nltk
from nlp_id.lemmatizer import Lemmatizer 
import pandas as pd
import json
from sklearn.preprocessing import LabelEncoder
# import dataset
with open('Dataset\Data_Wahana.json') as content:
  data1 = json.load(content)

# Mendapatkan semua data ke dalam list
tags = [] # data tag
inputs = [] # data input atau pattern
responses = {} # data respon
classes = [] # Data Kelas atau Tag
documents = [] # Data Kalimat Dokumen

for intent in data1['intents']:
  responses[intent['tag']]=intent['responses']
  for lines in intent['patterns']:
    inputs.append(lines)
    tags.append(intent['tag'])
    for pattern in intent['patterns']:
      # add to our classes list
      if intent['tag'] not in classes:
        classes.append(intent['tag'])

data = pd.DataFrame({"patterns":inputs, "tags":tags})

le = LabelEncoder()
data['tags'] = le.fit_transform(data['tags'])

key_norm = pd.read_csv('Dataset\key_norm_v2.csv')
lemmatizer= Lemmatizer()
more_stopword = ['bang', 'savi']

# Lakukan casefolding
def casefolding(text):
  text = text.lower()                               # Mengubah teks menjadi lower case
  text = re.sub(r'https?://\S+|www\.\S+', '', text) # Menghapus URL
  text = re.sub(r'[-+]?[0-9]+', '', text)           # Menghapus angka
  text = re.sub(r'[^\w\s]','', text)                # Menghapus karakter tanda baca
  text = text.strip()
  return text

# Fungsi untuk Menormalisasi Text
def text_normalize(text):
  text = ' '.join([key_norm[key_norm['singkat'] == word]['hasil'].values[0] if (key_norm['singkat'] == word).any() else word for word in text.split()])
  text = str.lower(text)
  text = re.sub(r'(.)\1+', r'\1', text)
  return text

# Lakukkan lemmatizer
def lemma(text):
  text = lemmatizer.lemmatize(text)
  return text

def remove_stop_words(text):
  clean_words = []
  text = text.split()
  for word in text:
      if word not in more_stopword:
          clean_words.append(word)
  return ' '.join(clean_words)

# Fungsi untuk Text Pre-Processing
def text_preprocessing_process(text):
  text = casefolding(text)
  text = text_normalize(text)
  text = lemma(text)
  text = remove_stop_words(text)
  return text