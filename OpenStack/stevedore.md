# Stevedore  작동원리



## 커맨드들은 어떻게 수집하는가

```ini
# setup.cfg

[entry_points]
console_scripts =
    openstack = openstackclient.shell:main

openstack.cli =
    command_list = openstackclient.common.module:ListCommand
    module_list = openstackclient.common.module:ListModule

openstack.cli.base =
    compute = openstackclient.compute.client
    identity = openstackclient.identity.client
    image = openstackclient.image.client
    network = openstackclient.network.client
    object_store = openstackclient.object.client
    volume = openstackclient.volume.client

openstack.common =
    availability_zone_list = openstackclient.common.availability_zone:ListAvailabilityZone
    configuration_show = openstackclient.common.configuration:ShowConfiguration
    extension_list = openstackclient.common.extension:ListExtension
    extension_show = openstackclient.common.extension:ShowExtension
    limits_show = openstackclient.common.limits:ShowLimits
    project_cleanup = openstackclient.common.project_cleanup:ProjectCleanup
    project_purge = openstackclient.common.project_purge:ProjectPurge
    quota_list = openstackclient.common.quota:ListQuota

....

```



setup.cfg 에 등록되어 있는 모듈들. 



```python
# 가장먼저 프로젝트에서 하는 일 
# setup.cfg 에 있는 내용일 읽어서 설치하는 것 
# stevdore 는 설치되어있는 Entry Point 정보를 읽어서 (ExtensionManager 를 통해) namespace 지정  

python setup.py develop
```



```python
# stevedore 에게 namespace 를 넘겨준다. 

	def load_commands(self, namespace):
        """Load all the commands from an entrypoint"""
        self.group_list.append(namespace)			# namespace = {str} 'openstack.compute.v2'
        for ep in stevedore.ExtensionManager(namespace):
            LOG.debug('found command %r', ep.name)
            cmd_name = (ep.name.replace('_', ' ')
                        if self.convert_underscores
                        else ep.name)
            self.commands[cmd_name] = ep.entry_point
        return

      
-------------------------------------
      
# setup.cfg
openstack.compute.v2 =			# 이게 namespace 이다. 
    compute_agent_create = openstackclient.compute.v2.agent:CreateAgent
    compute_agent_delete = openstackclient.compute.v2.agent:DeleteAgent
    compute_agent_list = openstackclient.compute.v2.agent:ListAgent
...
```



## Stevedore 는 어떻게 Entry point 정보를 찾아서 읽을 수 있을까? 



 setup.cfg 에 있는 내용을 기반으로 python-openstackclient.egg-info 가 생성된다. 

- python 의 모듈이 동작하는 방법
- setup.cfg 가 작동하는 과정 
- 패키지가 설치되는 과정 



