import requests
import json

def web_request(method_name, url, dict_data, is_urlencoded=True):
	method_name = method_name.upper()
	if method_name == 'GET':
		response = requests.get(url=url, params=dict_data)
	elif method_name == 'HEAD':
		response = requests.head(url=url, params=dict_data)
	elif method_name == 'POST':
		if is_urlencoded is True:
			response = requests.post(url=url, data=dict_data, headers={'Content-Type': 'application/x-www-form-urlencoded'})
		else:
			response = requests.post(url=url, data=json.dumps(dict_data), headers={'Content-Type': 'application/json'})
	elif method_name == 'PUT':
		if is_urlencoded is True:
			response = requests.put(url=url, data=dict_data, headers={'Content-Type': 'application/x-www-form-urlencoded'})
		else:
			response = requests.put(url=url, data=json.dumps(dict_data), headers={'Content-Type': 'application/json'})
	elif method_name == 'DELETE':
		if is_urlencoded is True:
			response = requests.delete(url=url, data=dict_data, headers={'Content-Type': 'application/x-www-form-urlencoded'})
		else:
			response = requests.delete(url=url, data=json.dumps(dict_data), headers={'Content-Type': 'application/json'})
	else:
		raise Exception('method name should be GET or POST...')


	dict_meta = {'status_code':response.status_code, 'ok':response.ok, 'encoding':response.encoding, 'Content-Type': response.headers['Content-Type']}
	if 'json' in str(response.headers['Content-Type']): # JSON 형태인 경우
		return {**dict_meta, **response.json()}
	else: # 문자열 형태인 경우
		return {**dict_meta, **{'text':response.text}}


url = 'http://192.168.50.197'

#GET

get_data = {'get_key':'this is get method content!'}
get_response = web_request(method_name='GET', url=url, dict_data=get_data)

print(get_response)

if get_response['ok'] == True:
	print('GET Success!')
	print(get_response['text'])
else:
	print('GET Fail!')

#HEAD

head_data= {'head_key':'this is head method content!'}
head_response = web_request(method_name='HEAD', url=url, dict_data=head_data)

print(head_response)

if head_response['ok'] == True:
	print('HEAD Success!')
	print(head_response['text'])
else:
	print('HEAD Fail!')


#POST

post_data = {'post_key':'this is post method content!'}
post_response = web_request(method_name='POST', url=url, dict_data=post_data, is_urlencoded=False)

print(post_response)
if post_response['ok'] == True:
	print('POST Success!')
	print(post_response['text'])
else:
	print('POST Fail!')


#PUT

put_data = {'put_key':'this is put method content!'}
put_response = web_request(method_name='PUT', url=url, dict_data=put_data, is_urlencoded=False)

print(put_response)
if put_response['ok'] == True:
	print('PUT Success!')
	print(put_response['text'])
else:
	print('PUT Fail!')

#DELETE

delete_data = {'delete_key':'this is delete method content!'}
delete_response = web_request(method_name='DELETE', url=url, dict_data=delete_data, is_urlencoded=False)

print(delete_response)
if delete_response['ok'] == True:
	print('DELETE Success!')
	print(delete_response['text'])
else:
	print('DELETE Fail!')


