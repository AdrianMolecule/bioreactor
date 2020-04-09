gcloud auth login
gcloud projects create bioreactor
gcloud config set project bioreactor
gcloud alpha billing projects link bioreactor --billing-account 0193B1-3C16E1-ED8E85
gcloud services enable container.googleapis.com
gcloud services enable deploymentmanager.googleapis.com
