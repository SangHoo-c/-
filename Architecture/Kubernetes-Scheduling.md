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
# create nginx pod kubectl run nginx --image=nginx# Generate POD Manifest YAML file (-o yaml). Don't create it(--dry-run)kubectl run nginx --image=nginx --dry-run=client -o yaml# Create a deploymentkubectl create deployment --image=nginx nginx# Generate Deployment YAML file (-o yaml). Don't create it(--dry-run)kubectl create deployment --image=nginx nginx --dry-run=client -o yaml# Generate Deployment YAML file (-o yaml). Don't create it(--dry-run) with 4 Replicas (--replicas=4)kubectl create deployment --image=nginx nginx --dry-run=client -o yaml > nginx-deployment.yaml# --dry-run=clinet 를 통해 내가 만들어놓은 명령형 커멘드가 맞는지 확인 할 수 있다. 
~~~



```
kubectl create deployment --image=httpd:2.4-alpine httpd-frontend --replicas=3
```

```
kubectl create deployment httpd-frontend --image=httpd:2.4-alpinekubectl scale deployment --replicas=3 httpd-frontend
```



### NameSpace



```
# in default namespace mysql.connect("db-service")# when trying to connect db-service in dev namespacemysql.connect("db-service.dev.svc.cluster.local")mysql.connect(<service-name>.<namespace>.<service>.<domain>)
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
kubectl create -f service-definition.yamlkubectl get serviceskubectl get svckubectl descirbe svc kubernetes 	# kubernetes service 자세히 보기 curl http://192.168.1.2:30008	# testing # simple-webapp-deployment 이름의 deployment expose 하기, # target-port 8080, NodePort 8080, svc 이름은 webapp-servicekubectl expose deployment simple-webapp-deployment --name=webapp-service --target-port=8080 --type=NodePort --port=8080
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
# Create Objectskubectl apply -f nginx.yaml# Update Objectskubectl apply -f nginx.yaml# local 에 있는 file 을 수정하고 apply 하면 된다. 
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



## Scheduling





```yaml
# 스케줄러 정상 작동 중인지 확인 
kubectl -n kube-system get pods
```



### Labels and Selectors

**Labels** : add attribute

- application
- object
- Functionality

```yaml
# pod-definition.yml
# string value
apiVersion: v1
kind: Pod
# key-value, dictionary, 
metadata:
  name: myapp-pod
  # 원하는 만큼 label 을 넣을 수 있다. 
  labels:
    app: myapp
    type: front-end
spec:
  containers: # list/array
    - name: nginx-container # 1st item in list 
      image: nginx 


```

~~~
kubectl get pods -show-labelskubectl get pods --selector app=myappkubectl get all -l env=prodkubectl get pod -l env=prod,tier=frontend,bu=finance
~~~



**Selector** : filter

replicaSet, Service 에서 원하는 Pod 를 고를때 Selector

replicaSet >= Pods 

Pod의 Label은 replicaSet 에게 필요 



**Annotations** : detail info 



### Taints and Tolerations

노드를 제한하기 위해 만들어진 개념, 특정한 Pod 에게만 허용

**tip** 해당 pod 가 해당 node 로 가는걸 보장하진 않는다. 

어떤 노드에 어떤 파드가 배치될지, what pods schedule on the node 

1. Taints : 더러움, 노드에 칠해놓는다, pod 들이 피해간다. 
2. Toleration level : 관용, level 이 높으면 taints 도 이해한다. 



~~~yaml
# taint-effect : NoSchedule | PreferNoSchedule | NoExecutekubectl taint nodes <node-name> key=value:taint-effectkubectl taint nodes node1 app=blue:NoSchedulekubectl taint nodes node01 spray=mortein:NoSchedule# taint 삭제kubectl taint nodes node1 key1=value1:NoSchedule-kubectl taint nodes controlplane node-role.kubernetes.io/master:NoSchedule-# master node 의 정책kubectl describe node kubemaster | grep Taint# 결과 root@controlplane:~# kubectl describe node controlplane | grep TaintTaints:             node-role.kubernetes.io/master:NoScheduleroot@controlplane:~# kubectl describe node node01 | grep Taintroot@controlplane:~# kubectl describe node node01 | grep -i taintTaints:             spray=mortein:NoSchedule
~~~

~~~yaml
apiVersion: v1kind: Podmetadata:  name: myapp-podspec:  containers: # list/array    - name: nginx-container # 1st item in list       image: nginx 	# 명령형으로 써준 내용 이와 같다.   tolerations:  	- key: "app"  		operator: "Equal"  		value: blue  		effect: NoSchedule
~~~



######  pod 생성 후 변경

pod 를 생성한후, taint 되어 있는 노드를 대상으로 한 toleration 걸어주기 

```
kubectl run bee --image=nginx --dry-run=client -o yml > bee.ymlkubectl explain pod --recursive | lesskubectl explain pod --recursive | grep -A5 tolerations
```



### Node Selector 

~~~yaml
apiVersion: v1kind: Podmetadata:  name: myapp-podspec:  containers: # list/array    - name: nginx-container # 1st item in list       image: nginx 	nodeSelector:		size: large			# ??? 어디서 왔지, node 에 할당된 label 이다. scheduler 가 이를 체크한다. 
~~~

~~~
kubectl label nodes node-1 size=Large
~~~



**한계점**

- 여러개의 노드에 대해서 OR 효과를 주지 못한다.

- NOT Large 와 같은 조건을 주지 못한다. 

  ==> Node Affinity could solve this problem! 



### Node Affinity 

~~~yaml
apiVersion: v1kind: Podmetadata:  name: myapp-podspec:  containers: # list/array    - name: nginx-container # 1st item in list       image: nginx 		affinity:		nodeAffinity:			requiredDuringSchedulingIgnoredDuringExecution:				nodeSelectorTerms:				-	matchExpressions:					-	key: size						operator: In			# 여러개의 operator 있음 check 해보기						value:						-	Large
~~~



operator : In, Exists, 

DuringScheduling : pod 가 처음 생성되어, 스케줄러가 node 에 배치하려 할때

DuringExecution : pod 가 이미 running 중이고, environment 에 변화가 있을때, label 이 변경된 경우 



###### Available

- type1. **required**DuringScheduling  **Ignored**DuringExecution

pod 에 해당하는 node 를 찾지 못한다면, 스케줄러가 pod 를 스케줄 하지 않는다. 

pod 의 배치가 crucial 할때 사용.

- type2. **preferred**DuringScheduling  **Ignored**DuringExecution

matching 하는 node 를 찾지 못할 경우, 아무 node 에 배치 



###### Planned

- type3. requiredDuringScheduling  **Required**DuringExecution



```yaml
apiVersion: apps/v1kind: Deploymentmetadata:  name: bluespec:  selector:    matchLabels:      app: nginx  replicas: 3  template:    metadata:      labels:        app: nginx    spec:      containers:        - name: nginx          image: nginx      affinity:        nodeAffinity:          requiredDuringSchedulingIgnoredDuringExecution:            nodeSelectorTerms:              - matchExpressions:                - key: size                  operator: In                  value:                  - blue
```

```yaml
---apiVersion: apps/v1kind: Deploymentmetadata:  name: bluespec:  replicas: 3  selector:    matchLabels:      run: nginx  template:    metadata:      labels:        run: nginx    spec:      containers:      - image: nginx        imagePullPolicy: Always        name: nginx      affinity:        nodeAffinity:          requiredDuringSchedulingIgnoredDuringExecution:            nodeSelectorTerms:            - matchExpressions:              - key: color                operator: In                values:                - blue
```



pod  가 **할당된 node** 가 어딘지 체크 하기 

```yaml
kubectl get node node-01 --show-labelskubectl get pods -o wide# 현재 동작 중인 deployment을 yaml 파일로 나타내기 kubectl get deployment blue -o yaml > blue.yaml# yaml 코드 수정후에 해당 리소스 delete 해준후, 새로 적용한다. kubectl delete deployment bluekubectl apply -f blue.yaml
```



### Resource Request

```yaml
---apiVersion: apps/v1kind: Deploymentmetadata:  name: bluespec:  replicas: 3  selector:    matchLabels:      run: nginx  template:    metadata:      labels:        run: nginx    spec:      containers:      - image: nginx        imagePullPolicy: Always        name: nginx				resources:					requests:						memory: "1Gi"						cpu: 1					# 1 cpu = 1000 m 					limits: 						memory: "2Gi"						cpu: 2
```

container 별로 resource 요구한다. 



**Editing Pod tips**

#### Edit Deployments

With Deployments you can easily edit any field/property of the POD template. Since the pod template is a child of the deployment specification, with every change the deployment will automatically delete and create a new pod with the new changes. So if you are asked to edit a property of a POD part of a deployment you may do that simply by running the command

```
kubectl edit deployment my-deployment
```





### Daemon Sets

one copy of pod will exist on all node on cluster

- Monitoring Solution
- Logs Viewer 
- kube-proxy
- Networking



```yaml
apiVersion: apps/v1kind: DaemonSetmetadata:	name: monitoring-daemon	labels:		app: myapp		type: font-endspec:	template: 	# 생성될 pod 의 template     metadata: 		 name: monitoring-agent	   labels:   		 app: myapp	     type: front-end # selector 와 일치해야 한다. 		 spec:		   containers: # list/array			    - name: nginx-container # 1st item in list  	      		image: nginx  	selector: 		matchLabels:			type: monitoring-agent  # template dml label 과 일치해야 한다. 
```

```
kubectl create -f dmset.yaml
```



***How does it work?***

~~ v1.12 - set nodeName to pod 

v1.12 ~~ - nodeAffinity and default scheduler 



```yaml
# context switching # namespace	변경 kubectl config set-context --current --namespace=default
```





### Static Pods

node 하나만 있다면? 

master 노드가 없다면? etdc 가 없다면?

kubelet 이 얼만큼의 역할을 할까?



kubelet 은 pod 를 create 할 수 있다. 

하지만 api server 가 없다면? 

kublet 가 pod.yaml 을 읽도록 설정해야한다. 



**오직** pod 만 이러한 방식으로 ***독립적으로*** 생성 가능하다. 

- created by Kubelet
- deploy control plane components as Static Pods 
- Ignore by the Kube-Scheduler 

***tip***     static pod 는 pod 이름 뒤에 node 이름이 붙어 있다. 

Run the command `ps -aux | grep kubelet` and identify the config file - `--config=/var/lib/kubelet/config.yaml`. Then check in the config file for staticPodPath.



```yaml
# kubectl run --restart=Never --image=busybox static-busybox --dry-run=client -o yaml --command -- sleep 1000 > /etc/kubernetes/manifests/static-busybox.yamlapiVersion: v1kind: Podmetadata:  creationTimestamp: null  labels:    run: static-busybox  name: static-busyboxspec:  containers:  - command:    - sleep    - "1000"    image: busybox    name: static-busybox    resources: {}  dnsPolicy: ClusterFirst  restartPolicy: Neverstatus: {}
```



Identify which node the static pod is created on, ssh to the node and delete the pod definition file.
If you don't know the IP of the node, run the `kubectl get nodes -o wide` command and identify the IP.
Then, SSH to the node using that IP. For static pod manifest path look at the file `/var/lib/kubelet/config.yaml` on node01

Next, SSH to node01 and identify the path configured for static pods in this node.
`Important`: The path need not be `/etc/kubernetes/manifests`. Make sure to check the path configured in the kubelet configuration file.

```apl
root@controlplane:~# ssh node01 							 or ~# ssh <IP address of node>root@node01:~# ps -ef |  grep /usr/bin/kubelet root       752   654  0 00:30 pts/0    00:00:00 grep --color=auto /usr/bin/kubeletroot     28567     1  0 00:22 ?        00:00:11 /usr/bin/kubelet --bootstrap-kubeconfig=/etc/kubernetes/bootstrap-kubelet.conf --kubeconfig=/etc/kubernetes/kubelet.conf --config=/var/lib/kubelet/config.yaml --network-plugin=cni --pod-infra-container-image=k8s.gcr.io/pause:3.2root@node01:~# grep -i staticpod /var/lib/kubelet/config.yamlstaticPodPath: /etc/just-to-mess-with-youroot@node01:~# 
```

Here the staticPodPath is `/etc/just-to-mess-with-you`
Navigate to this directory and delete the YAML file:

```apl
root@node01:/etc/just-to-mess-with-you# lsgreenbox.yamlroot@node01:/etc/just-to-mess-with-you# rm -rf greenbox.yaml root@node01:/etc/just-to-mess-with-you#
```

Exit out of node01 using `CTRL + D` or type `exit`. You should return to the `controlplane` node. Check if the `static-greenbox` pod has been deleted:

***정리***

1. kubelet config 파일의 위치를 찾는다
2. config.yaml 안에서 static 경로를 찾는다. 
3. 해당 경로에서 pod 생성/삭제 한다. 





### Multiple Schedulers

Scheduler 배포 방법 

https://stackoverflow.com/questions/65942744/how-to-access-kube-scheduler-on-a-kubernetes-cluster

1. 직접 binary file 다운로드 
2. kubeadm 사용 => Static Pod 수정

2번의 방법을 사용

```yaml
# custom scheduler # binary file 다운로드wget ~~~ kube-scheduler.service... --scheduler-name=<custom-name># multiple 스케줄러에서 only one active 가능 ... --leader-elect-true
```

~~~yaml
# /etc/kubernets/manifest 안에 위치한 kube-scheduler 파일 수정 who makes kube-scheduler.yaml ?! 
~~~



~~~yaml
# pod-definition.yml# string valueapiVersion: v1kind: Pod# key-value, dictionary, metadata:  name: myapp-podspec:  containers: # list/array    - name: nginx-container # 1st item in list       image: nginx   schedulerName: <custom-scheduler>
~~~

~~~
kubectl get eventskubectl logs <custom-scheduler> -n=kube-system
~~~



### Logging & monitoring 

- METRICS SERVER : in-memory, 디스크에 저장 하지 않는다. (추가적인 loggin solution 필요)
- 각 Node 의 kubelet 에이전트안에 cAdvisor 라는 컴포넌트에서 수집한다. 
- 수집된 정보는 Metric Server 로 전송한다. 



Metrics Server 시작 하는 방법

1. git 에서 다운로드 받기 



~~~
kubectl top nodekubectl top pod 
~~~



###### Docker logs

```
docker run -d hoo/event-simulatordocker logs -f <container-id>
```



###### K8s logs

```
kubectl run -f event-simulator.yamlkubectl logs -f <pod-name>
```



***만약 pod 에 여러개의 container 가 있다면?***

~~~yaml
# event-simulator.yaml# string valueapiVersion: v1kind: Pod# key-value, dictionary, metadata:  name: myapp-podspec:  containers: # list/array    - name: nginx-container # 1st item in list       image: nginx    - name: nginx-container2 # 2st item in list       image: nginx
~~~

```yaml
# you need to specify which container you want to view log.kubectl logs -f <pod-name> nginx-container2
```



## Application LifeCycle Management



### Rolling Update & Rollback

```yaml
# checking history kubectl rollout status <deployment-history>
```



- Recreate : application down 이 존재 
- Rolling Update : default 



~~~yaml
kubectl apply -f new-deployment.yaml# 해당 경우 yaml 파일은 변경되지 않는다. # 꼭, 추후에 변경 해줘야한다, kubectl set image deployment/new-deployment nginx=nginx:1.9.1
~~~



```yaml
# createkubectl create -f deployment-definition.yaml# Get kubectl get deployments# Update  kubectl apply –f deployment-definition.yaml kubectl set image deployment/myapp-deployment nginx=nginx:1.9.1  # Status  kubectl rollout status deployment/myapp-deployment kubectl rollout history deployment/myapp-deployment  # Rollback kubectl rollout undo deployment/myapp-deployment  # Edit kubectl edit deployment frontend
```





```yaml
# change deployment strategy kubectl patch deployment/frontend --patch '{"spec":{"strategy":{"type":"Recreate"}}}'
```



### Configure Applications

Configuring applications comprises of understanding the following concepts:

- Configuring Command and Arguments on applications
- Configuring Environment Variables
- Configuring Secrets



~~~dockerfile
# ubuntu-sleeperFROM UbuntuENTRYPOINT ["sleep"]CMD ["5"]-------# pod-definition.yml# string valueapiVersion: v1kind: Pod# key-value, dictionary, metadata:  name: myapp-podspec:  containers: # list/array    - name: ubuntu-sleeper       image: ubuntu-sleeper      command:["sleep2.0"]      args:["10"]
~~~



### ENV variables in K8S

```yaml
# pod-definition.yml# string valueapiVersion: v1kind: Pod# key-value, dictionary, metadata:  name: myapp-podspec:  containers: # list/array    - name: nginx-container # 1st item in list       image: nginx             env:	# array, key-value        -  name: APP_COLOR        	 value: blue
```



1. Plain key-value

~~~yaml
      env:	# array, key-value        -  name: APP_COLOR        	 value: blue
~~~

### ConfigMap

```yaml
      env:	# array, key-value        -  name: APP_COLOR        	 valueFrom:         	     configMapKeyRef:
```



- pod 밖으로 빼서 관리 
- pod 생성시에 inject 한다. 

```yaml
# 명령형 선언 kubectl create configmap <config-name> --from-literal=<key>=<value>kubectl create configmap app-config --from-literal=APP_COLOR=bluekubectl create configmap webapp-config-map --from-literal=APP_COLOR=darkblue=# 명령형 선언 <파일에 저장>kubectl create configmap app-config --from-file=app_config.properties# declarative kubectl create -f config-map.yaml
```



- create Config map 

```yaml
# config-map.yamlapiVersion: v1kind: ConfigMap# key-value, dictionary, metadata:  name: app-config data:  APP_COLOR: blue  APP_MODE: prod
```



- inject ConfigMap in Pods

```yaml
# pod-definition.ymlapiVersion: v1kind: Pod# key-value, dictionary, metadata:  name: myapp-podspec:  containers: # list/array    - name: nginx-container # 1st item in list       image: nginx             envFrom:	# array, key-value        -  configMapRef:        	   name: app-config
```



### Secrets 

```yaml
      env:	# array, key-value        -  name: APP_COLOR        	 valueFrom:        	     secretKeyRef:
```



* Tip : 현재 떠있는 pod 의 yaml 파일 가져오기 

~~~yaml
kubectl get pod <pod-name> - o yaml > pod.yaml
~~~



- create Secret 
- inject in pod

```yaml
# 명령형 선언kubectl create secret generic app-secret --from-literal=DB_HOST=mysqlkubectl create secret generic <secret-name> --from-literal=<key>=<value>kubectl create secret generic <secret-name> --from-file=<path-to-file># base64 utility 를 사용하여 암호화 진행 echo -n 'mysql' | base64 kubectl get secret app-secret -o yaml# envFrom 정보 체크# explain 명령어를 통해 정보를 얻을 수 있다!! kubectl explain pods --recursive | less
```





