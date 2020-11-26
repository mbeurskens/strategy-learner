function [] = set_permissions( config )
%SET_PERMISSIONS Set permissions so normal user can use results
        source_exp_dir = strsplit(config.exp_dir, '/');
        system(['chown -R robocup ' source_exp_dir{1}]);

end

