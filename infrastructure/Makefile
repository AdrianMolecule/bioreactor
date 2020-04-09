update:
	gcloud deployment-manager deployments update cluster --config cluster.yaml

create:
	gcloud deployment-manager deployments create cluster --config cluster.yaml

delete:
	gcloud deployment-manager deployments delete cluster --config cluster.yaml

describe:
	gcloud deployment-manager resources describe bioreactor-cluster --deployment cluster

setup-kubectl:
	gcloud container clusters get-credentials bioreactor-cluster --zone us-east4-a
