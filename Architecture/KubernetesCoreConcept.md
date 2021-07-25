### CKA

**Below are some references:**

Certified Kubernetes Administrator: https://www.cncf.io/certification/cka/

Exam Curriculum (Topics): https://github.com/cncf/curriculum

Candidate Handbook: https://www.cncf.io/certification/candidate-handbook

Exam Tips: http://training.linuxfoundation.org/go//Important-Tips-CKA-CKAD

> Use the code - **DEVOPS15** - while registering for the CKA or CKAD exams at Linux Foundation to get a 15% discount.



### kube-scheduler

스케줄러는 어떤 노드에 파드가 갈지 결정 

실제 파드는 Kublet 이 배치한다. 

pod 에 가장 적당한 노드를 찾는다. 

1. Filter nodes 
2. Rank Nodes 



### kubectl

```
kubectl run nginx --image nginx
kubectl get pods 
kubectl describe pod myapp-pod
```



### Pod 

```yaml
# pod-definition.yml
# string value
apiVersion: v1
kind: Pod
# key-value, dictionary, 
metadata:
  name: myapp-pod
  labels:
    app: myapp
    type: front-end
spec:
  containers: # list/array
    - name: nginx-container # 1st item in list 
      image: nginx 

```

```
cat > pod-definition.yml
kubectl create -f pod-definition.yml
```



### multiple Pods

```yaml
# pod-definition.yml
# string value
apiVersion: v1
kind: Pod
# key-value, dictionary, 
metadata:
  name: myapp-pod
  labels:
    app: myapp
    type: front-end
spec:
  containers: # "-" means list/array
    - name: nginx-container # 1st item in list 
      image: nginx 
    - name: log-agent				# 2st item in list 
      image: log-agent:v1


```



**Debugging with container exec**

```yaml
kubectl exec ${POD_NAME} -c ${CONTAINER_NAME} -- ${CMD} ${ARG1} ${ARG2} ... ${ARGN}

# As an example, to look at the logs from a running Cassandra pod, you might run
kubectl exec cassandra -- cat /var/log/cassandra/system.log
kubectl -n elastic-stack exec app -- tail -f /log/app.log

# Pod 에 대한 log 수집의 경우
kubectl logs <pod-name>
```



### Multi-container Pods Design Patterns

1. sidecar 패턴 : 로깅에 사용
2. adapter 패턴
3. ambassador 패턴 



**initContainer**

But at times you may want to run a process that runs to completion in a container. For example a process that pulls a code or binary from a repository that will be used by the main web application. That is a task that will be run only one time when the pod is first created. Or a process that waits for an external service or database to be up before the actual application starts. That's where **initContainers** comes in.

하나의 프로세스 실행 전 단계가 필요한 경우, init container 를 사용 

https://kubernetes.io/docs/concepts/workloads/pods/init-containers/



**명령형으로 pod 만들기**

~~~
kubectl run nginx --image=nginx
~~~

```
kubectl create service -o yaml --dry-run=client
kubectl run redis --image=redis123 --dry-run=client -o yml > pod.yml
```



**pod/redis 의 이미지를 바꾸는 방법 **

https://stackoverflow.com/questions/40366192/kubernetes-how-to-make-deployment-to-update-image

Container name : redis

image name : redis 

```yaml
# kubectl patch pod <podname> -p '{"spec":{"template":{"spec":{"terminationGracePeriodSeconds":31}}}}'
kubectl patch pod redis -p '{"spec":{"template":{"spec":{"terminationGracePeriodSeconds":31}}}}'
```

```yaml
# kubectl set image pod/<pod-name> <container-name>=<image-name>
kubectl set image pod/redis redis=redis
```

또는

~~~yaml
# 1. update the pod-definition file and use kubectl apply command 
# 2. use kubectl edit pod redis command 

# edit running configuration
kubectl edit pod redis
...

~~~



### Replication Controller

- High Availabilty 
- Load Balancing & Scaling 

Replication Controller **vs** Replica Set 

```yaml
# rc-definition.yml
apiVersion:v1
kind:ReplicationController
metadata:
  name: myapp-rc
  labels:
   app: myapp
   type: font-end
spec:
  template:
  # pod-definition.yml 에 사용했던 내용 그대로 복붙
    metadata:
 		 name: myapp-pod
	   labels:
   		 app: myapp
	     type: front-end
		 spec:
		   containers: # list/array
			    - name: nginx-container # 1st item in list 
 	      		image: nginx  
replicas: 3
	 
```



~~~yaml
# replicaset-definition.yml # replicaset 의 api version 은 apps/v1 이다. 
apiVersion: apps/v1
kind:ReplicaSet
metadata:
	name: myapp-replicaset
	labels:
		app: myapp
		type: font-end
spec:
	template: 
	  # pod-definition.yml 에 사용했던 내용 그대로 복붙
    metadata:
 		 name: myapp-pod
	   labels:
   		 app: myapp
	     type: front-end # selector 와 일치해야 한다. 
		 spec:
		   containers: # list/array
			    - name: nginx-container # 1st item in list 
 	      		image: nginx  
	
replcas: 3
# rc can control pod which is not created by replicaset. 
# major difference between rc / replicaset 
selector: 
	matchLabels:
		type: front-end  # template dml label 과 일치해야 한다. 
~~~



### Lables and Selectors 

how to know which pods to monitoring 

Table => filter for replicaset. 

by using label. replicaset knows which pod to monitor. 

 

### Scale 

Case 1.

```yaml
# change yaml file
kubectl replace -f rc.yml
```

Case 2.

```yaml
kubectl scale --replicas=6 rc.yml

# or 

kubectl scale --replicas=6 <type> <name>
kubectl scale --replicas=6 replicaset myapp-replicaset
```



```yaml
kubectl set image replicaSet/new-replica-set <container-name>=<image-name>
kubectl set image replicaSet/new-replica-set busybox-container=busybox
```

또는 

~~~yaml
kubectl edit replicaset new-replica-set
~~~

이후 pod 모두 삭제 => 자동으로 대체됨 



### Deployment

- rolling update
- roll back
- pause - resume changes //multiple change in once (upgrading version, scale )

```yaml
# deployment-definition.yaml 
# replicaset 과 거의 동일하다. 
apiVersion: apps/v1
kind:Deployment
metadata:
	name: myapp-deployment
	labels:
		app: myapp
		type: font-end
spec:
	template: 
	  # pod-definition.yml 에 사용했던 내용 그대로 복붙
    metadata:
 		 name: myapp-pod
	   labels:
   		 app: myapp
	     type: front-end # selector 와 일치해야 한다. 
		 spec:
		   containers: # list/array
			    - name: nginx-container # 1st item in list 
 	      		image: nginx  
	
replcas: 3
# rc can control pod which is not created by replicaset. 
# major difference between rc / replicaset 
selector: 
	matchLabels:
		type: front-end  # template dml label 과 일치해야 한다. 
```



~~~
kubectl get deployments
kubectl get replicaset
kubectl get pods
~~~



```
kubectl get all
```



### 명령형 선언

kubectl run 

https://kubernetes.io/docs/reference/kubectl/conventions/  # 시험에 유용한 페이지 

~~~
# create nginx pod 
kubectl run nginx --image=nginx

# Generate POD Manifest YAML file (-o yaml). Don't create it(--dry-run)
kubectl run nginx --image=nginx --dry-run=client -o yaml

# Create a deployment
kubectl create deployment --image=nginx nginx

# Generate Deployment YAML file (-o yaml). Don't create it(--dry-run)
kubectl create deployment --image=nginx nginx --dry-run=client -o yaml

# Generate Deployment YAML file (-o yaml). Don't create it(--dry-run) with 4 Replicas (--replicas=4)
kubectl create deployment --image=nginx nginx --dry-run=client -o yaml > nginx-deployment.yaml

# --dry-run=clinet 를 통해 내가 만들어놓은 명령형 커멘드가 맞는지 확인 할 수 있다. 
~~~



```
kubectl create deployment --image=httpd:2.4-alpine httpd-frontend --replicas=3
```

```
kubectl create deployment httpd-frontend --image=httpd:2.4-alpine
kubectl scale deployment --replicas=3 httpd-frontend
```



### NameSpace



```
# in default namespace 
mysql.connect("db-service")

# when trying to connect db-service in dev namespace
mysql.connect("db-service.dev.svc.cluster.local")
mysql.connect(<service-name>.<namespace>.<service>.<domain>)
```



```
kubectl get pods --name=kube-system
kubectl get pods --all-namespace

# context switch
# context. 추후에 다시 다룰 예정 
kubectl config set-context <current-context> --naemspace=prod
kubectl config set-context ($kubectl kubectl config current-context) --namespace=prod
```

```
kubectl creaet namespace dev
```

```
kubectl create pod pod.yaml --namespace=dev

# 명령형으로 pod 실행 
kubectl run redis --image=redis -n=finance
```



### Service

connect application together 

**NodePort** : 외부에 노출하는 것에 중점 

view point of service 

```yaml
#service-definition.yaml
apiVersion: v1
kind: Service
metadata: 
	name: myapp-service

# crucial part of file, differ with different objects 
spec: 
    type: NodePort
    ports:
      - targetPort: 80
        port: 80
        nodePort: 30008

# There is nothing in here connect service to pod. => use Label & Selector ! 
    selectors:
        app: myapp
        type: front-end
```

```yaml
kubectl create -f service-definition.yaml
kubectl get services
kubectl get svc
kubectl descirbe svc kubernetes 	# kubernetes service 자세히 보기 
curl http://192.168.1.2:30008	# testing 

# simple-webapp-deployment 이름의 deployment expose 하기, 
# target-port 8080, NodePort 8080, svc 이름은 webapp-service
kubectl expose deployment simple-webapp-deployment --name=webapp-service --target-port=8080 --type=NodePort --port=8080
```

- 하나의 노드에 (같은 label) 여러개의 Pod 가 있는 경우 : service 가 생성, 랜덤 배치 
- 여러개의 노드에 (같은 label) 여러개의 Pod 가 있는 경우 : service 가 생성, NodePort 가 모든 노드에 열림, 



**ClusterIP** : 내부에서 서로 통신하는 것에 초점  

service 는 pod 를 하나로 묶어 single interface 를 제공

```yaml
#service-definition.yaml
apiVersion: v1
kind: Service
metadata: 
	name: back-end

spec: 
    type: ClusterIP 	# default type 
    ports:
      - targetPort: 80		# back-end is exposed 
        port: 80					# service is exposed

# There is nothing in here connect service to pod. => use Label & Selector ! 
    selectors:
        app: myapp
        type: back-end
```



**LoadBalancer** - 사용자 접근에 초점 

클러스터 안의 모든 노드에 NodePort 로 열린 모든 포트로 접근 가능 

사용자는 URL 로 접근하고 싶다. 

- 방법1. HAProxy, NGINX 와 같은 external Load Balancing VM에 설치, 설정 / 관리 
- 방법2. Cloud Provider 에서 제공하는 LB 사용 



### Imperative vs Declarative



**Imperative**

```yaml
# Imperative Commands 
# 간단하지만, 휘발성 

# Create Objexts
kubectl run --image=nginx nginx
kubectl create deployment --image=nginx nginx --replicas=3			# Generate Deployment with 3 Replicas
kubectl expose deployment nginx --port 80

# Update Objects 
kubectl edit deployment nginx			
kubectl scale deployment nginx --replicas=5						
kubectl set image deployment nginx nginx=nginx:1.18


# Imperative Object Configuration File 
# 정리해놓을 수 있다. 
kubectl create -f nginx.yaml
kubectl replace -f nginx.yaml
kubectl delete -f nginx.yaml
```



**Declarative**

```yaml
# Create Objects
kubectl apply -f nginx.yaml

# Update Objects
kubectl apply -f nginx.yaml

# local 에 있는 file 을 수정하고 apply 하면 된다. 
```



**Tips**

```
--dry-run=client : By default as soon as the command is run, the resource will be created. 
If you simply want to test your command, use the --dry-run=client option. 
This will not create the resource, instead, tell you whether the resource can be created 
and if your command is right.

-o yaml: This will output the resource definition in YAML format on the screen.
```

```
kubectl run redis --image=redis:alpine -l=tier=db

kubectl expose pod redis --name=redis-service --port=6379

kubectl run httpd --image=httpd:alpine --port 80 --expose 
```



Local file <=> Last applied Configuration <=> Kubernetes 


