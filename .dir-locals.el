((nil . ((eval . (add-to-list 'auto-mode-alist '("\\.h\\'" . c++-mode)))))
 (c++-mode . ((c-basic-offset . 4)
              (fill-column . 90)
              (flycheck-gcc-language-standard . "gnu++17")
              (eval . (progn
                        (c-set-offset 'innamespace 0)

                        (defun do-shell (s)
                          ;; Helper for running a shell command and getting the first line
                          ;; of its output.
                          (substring (shell-command-to-string s) 0 -1))

                        (setq flycheck-gcc-include-path
                              (let* ((project-root
                                      (do-shell "git rev-parse --show-toplevel"))
                                     (relative-project-subdirs
                                      '("include"))
                                     (absolute-project-subdirs
                                      (mapcar
                                       (lambda (subdir) (concat project-root "/" subdir))
                                       relative-project-subdirs)))
                                (absolute-project-subdirs))))))))
