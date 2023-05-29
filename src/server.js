const express = require('express');
const bodyParser = require('body-parser');
const { Client } = require('pg');

// Configuration de la base de données
const dbConfig = {
  user: 'postgres',
  host: 'localhost',
  database: 'essai',
  password: 'admin',
  port: 5432 // Port par défaut de PostgreSQL
};

// Création de l'instance du client PostgreSQL
const client = new Client(dbConfig);

// Connexion à la base de données
client.connect(err => {
  if (err) {
    console.error('Erreur de connexion à la base de données', err);
  } else {
    console.log('Connecté à la base de données PostgreSQL');
  }
});

// Création de l'application Express
const app = express();

// Middleware pour analyser les données JSON
app.use(bodyParser.json());

// Route pour recevoir les données du capteur CCS811
app.post('/donnees', (req, res) => {
  const { luminosite, co2, tvoc } = req.body;

  // Insérer les données dans la base de données
  const query = 'INSERT INTO donnee(luminosite, co2, tvoc) VALUES($1, $2, $3)';
  const values = [luminosite, co2, tvoc];

  client.query(query, values, (err, result) => {
    if (err) {
      console.error('Erreur lors de l\'insertion des données', err);
      res.status(500).json({ error: 'Erreur lors de l\'insertion des données' });
    } else {
      console.log('Données insérées avec succès');
      res.status(200).json({ message: 'Données insérées avec succès' });
    }
  });
});

// Démarrer le serveur
const port = 3000; // Vous pouvez modifier le port si nécessaire
app.listen(port, () => {
  console.log(`Serveur en écoute sur le port ${port}`);
});
