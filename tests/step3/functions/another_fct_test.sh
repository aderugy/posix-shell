# Je définis une première fonction

ecrire_sur_une_ligne() {
  echo -n $*
}

# Je définis une deuxième fonction qui appelle la première

saluer_utilisateur() {
  ecrire_sur_une_ligne "Bonjour "
  echo $UID
}

# J'appelle la deuxième fonction
saluer_utilisateur
