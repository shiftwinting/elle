# -*- encoding: utf-8 -*-

import json
import meta
import web

import metalib

from meta import conf, database
from meta.page import Page

class Device(Page):
    """
    Return all user's device ids
        GET /devices
            -> {
                'devices': [device_id1, ...],
            }

    Return one user device
        GET /device/id1
            -> {
                '_id': "id",
                'name': "pretty name",
                'addresses': [
                    {'ip': 'ip address 1', 'port': 1912},
                    ...
                ],
                'passport': "passport string",
            }

    Create a new device
        POST /device {
            'name': 'pretty name', # required
            'local_ip': 'local ip address',
            'local_port': 1912,
            'extern_port': 343,
        }
            -> {
                'success': True,
                'created_device_id': "id",
                'passport': "passport string",
            }

    Update an existing device
        POST /device {
            '_id': "id",
            'name': 'pretty name', # optional
            'local_ip': 'local ip address',
            'local_port': 1912,
            'extern_port': 343,
        }
            -> {
                'success': True,
                'updated_device_id': "id",
            }

    Delete a device
        DELETE /device/id
            -> {
                'success': True,
                'deleted_device_id': "id",
            }
    """

    def GET(self, id=None):
        self.requireLoggedIn()
        if id is None:
            return self.success({'devices': self.user.get('devices', [])})
        else:
            device = database.devices().find_one({
                '_id': database.ObjectId(id),
                'owner': self.user['_id'],
            })
            device.pop('owner')
            return self.success(device)

    def POST(self):
        self.requireLoggedIn()
        device = self.data
        if '_id' in device:
            func = self._update
        else:
            func = self._create

        return func(device)

    def _create(self, device):
        print("Create device", device)
        name = device.get('name', '').strip()
        if not name:
            return self.error("You have to provide a valid device name")

        to_save = {
            'name': name,
            'owner': self.user['_id'],
        }

        to_save['local_address'] = {
            'ip': device['local_ip'],
            'port': int(device.get('local_port', 0)),
        }

        to_save['extern_address'] = {
            'ip': web.ctx.env['REMOTE_ADDR'],
            'port': int(device.get('extern_port', to_save['local_address']['port'])),
        }

        id = database.devices().insert(to_save)
        assert id is not None

        to_save['passport'] = metalib.generate_passport(
            id,
            conf.INFINIT_AUTHORITY_PATH,
            conf.INFINIT_AUTHORITY_PASSWORD
        )
        database.devices().save(to_save)

        # XXX check unique device ?
        self.user.setdefault('devices', []).append(str(id))
        database.users().save(self.user)
        return self.success({
            'created_device_id': str(id),
            'passport': to_save['passport']
        })

    def _update(self, device):
        assert '_id' in device
        id = device['_id'].strip()
        if not id in self.user['devices']:
            raise web.Forbidden("This network does not belong to you")
        to_save = database.devices().find_one({
            '_id': database.ObjectId(id)
        })
        if 'name' in device:
            name = device['name'].strip()
            if not name:
                return self.error("You have to provide a valid device name")
            to_save['name'] = name

        if 'local_ip' in device:
            to_save['local_address']['ip'] = device['local_ip']
        if 'local_port' in device:
            to_save['local_address']['port'] = device['local_port']

        to_save['extern_address'] = {
            'ip': web.ctx.env.get('HTTP_REFERER'),
            'port': device.get('extern_port', to_save['local_address']['port']),
        }

        id = database.devices().save(to_save)
        return self.success({
            'updated_device_id': str(id),
            'passport': to_save['passport'],
        })

    def DELETE(self, id):
        self.requireLoggedIn()
        try:
            devices = self.user['devices']
            idx = devices.index(id)
            devices.pop(idx)
        except:
            return json.dumps({
                'success': False,
                'error': "The device '%s' was not found" % (id),
            })
        database.users().save(self.user)
        database.devices().find_and_modify({
            '_id': database.ObjectId(id),
            'owner': self.user['_id'], #not required
        }, remove=True)
        return self.success({
            'deleted_device_id': id,
        })

